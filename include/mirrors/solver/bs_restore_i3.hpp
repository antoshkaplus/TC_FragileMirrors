#pragma once
#include <vector>
#include <unordered_set>
#include <set>
#include <ranges>
#include <span>
#include "mirrors/common/position.hpp"
#include "mirrors/score/less.hpp"
#include "mirrors/board/board_n_i2.hpp"
#include "mirrors/board/board_n_i2_params.hpp"


namespace mirrors {

template <template<class> class Score>
class BS_Restore_i3 {
    using Board = Board_n_i2;
    using BoardParams = Board_n_i2_Params;
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
    explicit BS_Restore_i3(size_t beam_width,
                           const BoardGrid& mirrors,
                           Score<BoardParams> score = Score<BoardParams>()) :
            beam_width(beam_width),
            mirrors(mirrors),
            less(score) {}

    std::vector<Position> Destroy() {
        int size = mirrors.grid().size()-2;
        hashing = ZobristHashing(size);
        boards.emplace_back(Board_n_i2(size), BoardParams{});
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
            b.Cast(d.cast, d.params, mirrors);
            if (d.params.destroyed_count() == b.cell_count()) {
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