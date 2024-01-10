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

// Based on Solver_1. Replace dynamic beam width strategy based on `empty_lines`
// with beam width based on fixed total number of candidates next boards provide each level.
template <template<class> class Score, class Board, class BoardParams>
class Solver_2 {
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
    explicit Solver_2(size_t beam_width_init,
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
            visited.clear();
        }
        throw std::runtime_error("not found");
        return {};
    }

    void AddBoardDerivatives(BoardItem& item) {
        item.board.ForEachCastCandidate([&](const CastValue& p) {
            auto new_params = item.board.RestoreCast(
                    p, item.params, mirrors, hashing);
            if (!visited.insert(new_params.hash).second) {
                return; // visited already
            }
            derivs.emplace_back(new_params, &item.board, less.score(new_params), p);
        });
    }

    std::optional<std::vector<Position>> AddDerivativesNextBoards() {
        size_t have_candidates = 0;
        auto start = derivs.begin();
        auto end = start + std::min(derivs.size(),
                                    derivs_used);
        std::nth_element(start,
                         end-1,
                         derivs.end());
        std::sort(start, end);
        auto res = AddDerivativesNextBoards(start, end, have_candidates);
        if (res) {
            return res;
        }

        derivs_used = end - start;
        double distinct_rate = static_cast<double>(next_boards.size()) / derivs_used;

        auto min_candidates_per_board = board_candidates_init - 2 * (end-1)->params.empty_lines();
        auto candidates_left = level_candidates - have_candidates;
        auto need_distinct = candidates_left / min_candidates_per_board;
        auto need_derivs = static_cast<size_t>(need_distinct / distinct_rate);

        start = end;
        end += std::min(derivs.size() - derivs_used, need_derivs);
        if (start == end) {
            return {};
        }
        std::nth_element(start,
                         end-1,
                         derivs.end());
        std::sort(start, end);
        res = AddDerivativesNextBoards(start, end, have_candidates);
        if (res) {
            return res;
        }
        derivs_used += end - start;
        return {};
    }

    std::optional<std::vector<Position>> AddDerivativesNextBoards(std::vector<Derivative>::const_iterator start,
                                                                  std::vector<Derivative>::const_iterator end,
                                                                  size_t& have_candidates) {
        for (auto& d: std::span(start, end)) {
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
    std::unordered_set<hash_value_t> visited;
    std::vector<BoardItem> next_boards;
    std::vector<BoardItem> boards;
    std::vector<Derivative> derivs;
};

}