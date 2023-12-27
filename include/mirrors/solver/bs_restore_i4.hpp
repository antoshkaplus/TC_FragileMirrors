#pragma once
#include <vector>
#include <unordered_set>
#include <set>
#include <ranges>
#include <span>
#include "mirrors/common/position.hpp"
#include "mirrors/score/less.hpp"


namespace mirrors {

// Based on BS_Restore_i3. Before computing derivatives of a board, check if it has an isolated cell:
// Immediately destroy it and promote to the next level.
// Did not improve score.
template <template<class> class Score, class Board, class BoardParams>
class BS_Restore_i4 {
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
    explicit BS_Restore_i4(size_t beam_width,
                           const BoardGrid& mirrors,
                           Score<BoardParams> score = Score<BoardParams>()) :
            beam_width(beam_width),
            mirrors(mirrors),
            less(score) {}

    std::vector<Position> Destroy() {
        int size = mirrors.grid().size()-2;
        hashing = ZobristHashing(size);
        boards.emplace_back(Board(size), BoardParams{});
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
        return {};
    }

    void AddBoardDerivatives(BoardItem& item) {
        auto& b = item.board;
        for (board_size_t i = 0; i < b.size(); ++i) {
            if (b.single_row_mirror(i)) {
                Position p{i, -1};
                auto next = b.NextFromBorder(p);
                if (b.single_col_mirror(next.col)) {
                    b.DestroyIsolated(next, item.params, hashing);
                    next_boards.emplace_back(std::move(item));
                    return;
                }
            }
        }
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
    ZobristHashing hashing;
    const BoardGrid& mirrors;
    Less<Score<BoardParams>> less {};
    std::unordered_set<hash_value_t> visited;
    std::vector<BoardItem> next_boards;
    std::vector<BoardItem> boards;
    std::vector<Derivative> derivs;
};

}