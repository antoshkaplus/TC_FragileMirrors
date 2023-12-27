#pragma once
#include <vector>
#include <unordered_set>
#include <set>
#include <ranges>
#include <span>
#include <boost/format.hpp>
#include "mirrors/common/position.hpp"
#include "mirrors/common/grid.hpp"
#include "mirrors/common/origin_grid.hpp"
#include "mirrors/common/zobrist_hashing.hpp"
#include "mirrors/score/less.hpp"


namespace mirrors {

// Based on BS_Restore_i5. Can't promote twice based on greedy scoring.
// Did not improve score.
template <template<class> class Score, class Board, class BoardParams>
class BS_Restore_i6 {
    using ScoreValue = Score<BoardParams>::Value;
    using BoardGrid = OriginGrid<Grid<Mirror>>;

    struct Derivative {
        BoardParams params;
        const Board* board;
        ScoreValue score;
        Position cast;
        bool greedy = false;

        bool operator<(const Derivative& other) const {
            return score > other.score;
        }
    };

    struct BoardItem {
        Board board;
        BoardParams params;
    };

    struct CompareGreedy {
        bool operator()(const Derivative& d_1, const Derivative& d_2) const {
            return d_1.params.destroyed_count() > d_2.params.destroyed_count();
        }
    };

public:
    explicit BS_Restore_i6(size_t beam_width,
                           double greedy_ratio,
                           const BoardGrid& mirrors,
                           Score<BoardParams> score = Score<BoardParams>()) :
            beam_width(beam_width),
            mirrors(mirrors),
            less(score) {
        if (greedy_ratio > 1.) {
            auto msg = boost::format("greedy_ratio > 1: %.2f") % greedy_ratio;
            throw std::invalid_argument(msg.str());
        }
        greedy_width = beam_width * greedy_ratio;
    }

    std::vector<Position> Destroy() {
        int size = mirrors.grid().size()-2;
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
        item.board.ForEachCastCandidate([&](const Position& p) {
            auto new_params = item.board.RestoreCast(
                    p, item.params, mirrors, hashing);
            if (!visited.insert(new_params.hash).second) {
                return; // visited already
            }
            derivs.emplace_back(new_params, &item.board, less.score(new_params), p);
        });
    }

    std::optional<std::vector<Position>> AddDerivativesNextBoards() {
        auto end = derivs.begin()+std::min(derivs.size(), beam_width);
        std::nth_element(derivs.begin(),
                         end-1,
                         derivs.end());
        std::sort(derivs.begin(), end);
        for (auto& d: std::span(derivs.begin(), end)) {
            d.greedy = false;
        }

        auto best_destroyed = std::min_element(
                derivs.begin(), end, CompareGreedy())->params.destroyed_count();

        if (end != derivs.end() && greedy_width > 0) {
            std::nth_element(end, end + greedy_width - 1, derivs.end(), CompareGreedy());
            std::sort(end, end + greedy_width, CompareGreedy());

            auto end_insert = end;
            for (auto &d: std::span(end, end + greedy_width)) {
                if (d.params.destroyed_count() > best_destroyed && !d.greedy) {
                    d.greedy = true;
                    *(--end_insert) = d;
                }
            }
        }

        for (auto& d: std::span(derivs.begin(), end)) {
            auto b = *d.board;
            b.Cast(d.cast, mirrors);
            if (d.params.board_empty(b)) {
                return b.cast_history();
            }
            next_boards.emplace_back(std::move(b), d.params);
        }
        return {};
    }

private:
    size_t beam_width;
    size_t greedy_width;
    ZobristHashing hashing;
    const BoardGrid& mirrors;
    Less<Score<BoardParams>> less {};
    std::unordered_set<hash_value_t> visited;
    std::vector<BoardItem> next_boards;
    std::vector<BoardItem> boards;
    std::vector<Derivative> derivs;
};

}