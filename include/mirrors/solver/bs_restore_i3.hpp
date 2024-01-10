#pragma once
#include <vector>
#include <unordered_set>
#include <set>
#include <ranges>
#include <span>
#include "mirrors/common/position.hpp"
#include "mirrors/score/less.hpp"


namespace mirrors {

// Is made for Board_n_i2 board interface. Using `neighbour board` with Cast Restore
// logic created a great performance increase comparing with Board_i{version} implementations.
template <template<class> class Score, class Board, class BoardParams>
class BS_Restore_i3 {
    using ScoreValue = Score<BoardParams>::Value;
    using CastValue = Board::CastValue;
    using BoardGrid = OriginGrid<Grid<Mirror>>;

    struct Derivative {
        BoardParams params;
        const Board* board;
        ScoreValue score;
        CastValue cast;

        bool operator<(const Derivative& other) const {
            return score > other.score;
        }
    };

    struct BoardItem {
        Board board;
        BoardParams params;
    };

public:
    explicit BS_Restore_i3(size_t beam_width,
                           const BoardGrid& mirrors,
                           Score<BoardParams> score = Score<BoardParams>()) :
            beam_width(beam_width),
            mirrors(mirrors),
            less(score) {}

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
        return {};
    }

    void AddBoardDerivatives(BoardItem& item) {
        item.board.ForEachCastCandidate([&](const auto& p) {
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