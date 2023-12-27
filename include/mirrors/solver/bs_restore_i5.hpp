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

// Based on BS_Restore_i3. A part of width is dedicated to greedy destroyed only scoring.
// The rest - user provided scoring.
// Did not improve score.
template <template<class> class Score, class Board, class BoardParams>
class BS_Restore_i5 {
    using ScoreValue = Score<BoardParams>::Value;
    using BoardGrid = OriginGrid<Grid<Mirror>>;

    struct Derivative {
        BoardParams params;
        const Board* board;
        ScoreValue score;
        Position cast;

        bool operator<(const Derivative& other) const {
            return score > other.score;
        }
    };

    struct BoardItem {
        Board board;
        BoardParams params;
    };

public:
    explicit BS_Restore_i5(size_t beam_width,
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
        auto end_greedy = derivs.begin();
        auto end = end_greedy;
        if (greedy_width > 0) {
            end_greedy += std::min(derivs.size(), greedy_width);
            end = end_greedy;
            std::nth_element(derivs.begin(),
                             end_greedy-1,
                             derivs.end(),
                             [](auto& d_1, auto& d_2) {
                                 return d_1.params.destroyed_count() > d_2.params.destroyed_count();
                             });
        }
        int size = static_cast<int>(derivs.size()) - static_cast<int>(greedy_width);
        if (size > 0) {
            end += std::min(static_cast<size_t>(size), beam_width - greedy_width);
            std::nth_element(end_greedy,
                             end-1,
                             derivs.end());
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