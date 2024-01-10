#pragma once
#include <vector>
#include <unordered_set>
#include <set>
#include <ranges>
#include <span>
#include "mirrors/common/position.hpp"
#include "mirrors/common/grid.hpp"
#include "mirrors/common/origin_grid.hpp"
#include "mirrors/common/zobrist_hashing.hpp"
#include "mirrors/score/less.hpp"

namespace mirrors {

template <template<class> class Score, class Board, class BoardParams>
class Solver_3 {
    using ScoreValue = Score<BoardParams>::Value;
    using CastValue = Board::CastValue;
    using BoardGrid = Grid<mir_t>;

    struct Derivative {
        BoardParams params;
        const Board* board;
        ScoreValue score;
        CastValue cast;

        bool operator<(const Derivative& other) const {
            return score > other.score || (score == other.score && params.hash < other.params.hash);
        }
    };

    struct BoardItem {
        Board board;
        BoardParams params;

        bool operator<(const BoardItem& other) const {
            return params.score < other.params.score;
        }
    };

    struct LevelStats {
        size_t derivs_used;
        double distinct_rate;
        size_t have_candidates;
        size_t next_boards;
        ScoreValue min_score;
        ScoreValue max_score;
        double min_density;
        double max_density;
    };

public:
    explicit Solver_3(size_t beam_width_init,
                      const BoardGrid& mirrors,
                      Score<BoardParams> score = Score<BoardParams>()) :
            board_candidates_init(4*mirrors.size()),
            level_candidates(beam_width_init*board_candidates_init),
            derivs_used(beam_width_init),
            mirrors(mirrors),
            less(score) {}

    std::vector<Position> Destroy() {
        int size = mirrors.size();
        hashing = ZobristHashing(size);
        boards.emplace_back(Board(size), BoardParams{size});
        for (auto i = 0; i < size*size; ++i) {
            for (auto& item: boards) {
                AddBoardDerivatives(item);
            }
            auto res = AddDerivativesNextBoards();
            if (res) {
                return res.value();
            }
            derivs.clear();
            boards.clear();
            boards.swap(next_boards);
        }
        throw std::runtime_error("not found");
        return {};
    }

    void AddBoardDerivatives(BoardItem& item) {
        item.board.ForEachCastCandidate([&](const CastValue& p) {
            auto new_params = item.board.RestoreCast(
                    p, item.params, mirrors, hashing);
            derivs.emplace_back(new_params, &item.board, less.score(new_params), p);
        });
    }

    std::optional<std::vector<Position>> AddDerivativesNextBoards() {
        size_t have_candidates = 0;
        hash_value_t last_hash = 0;
        auto start = derivs.begin();
        auto end = start + std::min(derivs.size(),
                                    derivs_used);
        derivs_used = 0;
        std::nth_element(start,
                         end-1,
                         derivs.end());
        std::sort(start, end);
        auto res = AddDerivativesNextBoards(start, end, last_hash, have_candidates);
        if (res) {
            return res;
        }

        double distinct_rate = static_cast<double>(next_boards.size()) / derivs_used;

        auto min_candidates_per_board = board_candidates_init - 2 * (end-1)->params.empty_lines();
        auto candidates_left = level_candidates - have_candidates;
        auto need_distinct = candidates_left / min_candidates_per_board;
        auto need_derivs = static_cast<size_t>(need_distinct / distinct_rate);

        start = end;
        end += std::min(derivs.size() - derivs_used, need_derivs);
        if (start == end) {
            auto density = [&](const Derivative& d) {
                return static_cast<double>(mirrors.cell_count() - d.params.destroyed_count()) /
                    ((mirrors.size() - d.params.empty_cols()) * (mirrors.size() - d.params.empty_rows()));
            };
            auto min = [](double a_1, double a_2) {
                return std::min(a_1, a_2);
            };
            auto max = [](double a_1, double a_2) {
                return std::max(a_1, a_2);
            };
            auto min_density = std::transform_reduce(derivs.begin(), derivs.begin()+derivs_used,
                                                     std::numeric_limits<double>::max(), min, density);
            auto max_density = std::transform_reduce(derivs.begin(), derivs.begin()+derivs_used,
                                                     std::numeric_limits<double>::min(), max, density);
            // compare is reversed
            auto pair = std::minmax_element(derivs.begin(), derivs.begin()+derivs_used);
            level_stats_.emplace_back(derivs_used, distinct_rate, have_candidates, next_boards.size(),
                                      pair.second->score, pair.first->score,
                                      min_density, max_density);
            return {};
        }
        std::nth_element(start,
                         end-1,
                         derivs.end());
        std::sort(start, end);
        res = AddDerivativesNextBoards(start, end, last_hash, have_candidates);
        if (res) {
            return res;
        }

        auto density = [&](const Derivative& d) {
            return static_cast<double>(mirrors.cell_count() - d.params.destroyed_count()) /
                   ((mirrors.size() - d.params.empty_cols()) * (mirrors.size() - d.params.empty_rows()));
        };
        auto min = [](double a_1, double a_2) {
            return std::min(a_1, a_2);
        };
        auto max = [](double a_1, double a_2) {
            return std::max(a_1, a_2);
        };
        auto min_density = std::transform_reduce(derivs.begin(), derivs.begin()+derivs_used,
                                                 std::numeric_limits<double>::max(), min, density);
        auto max_density = std::transform_reduce(derivs.begin(), derivs.begin()+derivs_used,
                                                 std::numeric_limits<double>::min(), max, density);

        // compare is reversed
        auto pair = std::minmax_element(derivs.begin(), derivs.begin()+derivs_used);
        level_stats_.emplace_back(derivs_used, distinct_rate, have_candidates, next_boards.size(),
                                  pair.second->score, pair.first->score,
                                  min_density, max_density);
        return {};
    }

    std::optional<std::vector<Position>> AddDerivativesNextBoards(std::vector<Derivative>::const_iterator start,
                                                                  std::vector<Derivative>::const_iterator end,
                                                                  hash_value_t& last_hash,
                                                                  size_t& have_candidates) {
        for (auto& d: std::span(start, end)) {
            ++derivs_used;
            if (d.params.hash == last_hash) {
                continue;
            }
            last_hash = d.params.hash;
            auto b = *d.board;
            b.Cast(d.cast, mirrors);
            if (d.params.board_empty(b)) {
                return b.cast_history();
            }
            next_boards.emplace_back(std::move(b), d.params);
            have_candidates += board_candidates_init - 2 * d.params.empty_lines();
            if (have_candidates >= level_candidates) {
                break;
            }
        }
        return {};
    }

    const std::vector<LevelStats>& level_stats() const {
        return level_stats_;
    }

private:
    const size_t board_candidates_init;
    const size_t level_candidates;
    // Should not be initialized with 0.
    size_t derivs_used;
    ZobristHashing hashing;
    const BoardGrid& mirrors;
    Less<Score<BoardParams>> less {};
    std::vector<BoardItem> next_boards;
    std::vector<BoardItem> boards;
    std::vector<Derivative> derivs;
    std::vector<LevelStats> level_stats_;
};

}