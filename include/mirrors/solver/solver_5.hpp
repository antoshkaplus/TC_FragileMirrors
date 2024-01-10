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

// Based on Solver_4. Try to use more obvious inlining of compare functions
// in case compiler couldn't do it. Maybe minimal improvement.
template <template<class> class Score, class Board, class BoardParams>
class Solver_5 {
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
    };

public:
    explicit Solver_5(size_t beam_width_init,
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
            derivs_order.clear();
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
            derivs_order.push_back(derivs_order.size());
        });
    }

    std::optional<std::vector<Position>> AddDerivativesNextBoards() {
        size_t have_candidates = 0;
        hash_value_t last_hash = 0;
        auto start = derivs_order.begin();
        auto end = start + std::min(derivs_order.size(),
                                    derivs_used);
        std::nth_element(start,
                         end-1,
                         derivs_order.end(),
                         [&](const deriv_count_t i_1, const deriv_count_t i_2) {
                             return derivs[i_1] < derivs[i_2];
                         });
        std::sort(start, end, [&](const deriv_count_t i_1, const deriv_count_t i_2) {
            return derivs[i_1] < derivs[i_2];
        });
        auto res = AddDerivativesNextBoards(start, end, last_hash, have_candidates);
        if (res) {
            return res;
        }

        derivs_used = end - start;
        double distinct_rate = static_cast<double>(next_boards.size()) / derivs_used;

        auto i_end = *(end-1);
        auto min_candidates_per_board = board_candidates_init - 2 * derivs[i_end].params.empty_lines();
        auto candidates_left = level_candidates - have_candidates;
        auto need_distinct = candidates_left / min_candidates_per_board;
        auto need_derivs = static_cast<size_t>(need_distinct / distinct_rate);

        start = end;
        end += std::min(derivs_order.size() - derivs_used, need_derivs);
        if (start == end) {
            return {};
        }
        std::nth_element(start,
                         end-1,
                         derivs_order.end(),
                         [&](const deriv_count_t i_1, const deriv_count_t i_2) {
                             return derivs[i_1] < derivs[i_2];
                         });
        std::sort(start, end, [&](const deriv_count_t i_1, const deriv_count_t i_2) {
            return derivs[i_1] < derivs[i_2];
        });
        res = AddDerivativesNextBoards(start, end, last_hash, have_candidates);
        if (res) {
            return res;
        }
        derivs_used += end - start;
        return {};
    }

    std::optional<std::vector<Position>> AddDerivativesNextBoards(std::vector<deriv_count_t>::const_iterator start,
                                                                  std::vector<deriv_count_t>::const_iterator end,
                                                                  hash_value_t& last_hash,
                                                                  size_t& have_candidates) {
        for (auto& d_i: std::span(start, end)) {
            auto& d = derivs[d_i];
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
    std::vector<deriv_count_t> derivs_order;
};

}