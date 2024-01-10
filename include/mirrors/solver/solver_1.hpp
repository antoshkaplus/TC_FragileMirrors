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

// Based on BS_Restore_i7. Replace BoardGrid type. Border cells no longer needed.
template <template<class> class Score, class Board, class BoardParams, class BeamWidth>
class Solver_1 {
    using ScoreValue = Score<BoardParams>::Value;
    using CastValue = Board::CastValue;
    using BoardGrid = Grid<mir_t>;

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
    explicit Solver_1(BeamWidth beam_width,
                      const BoardGrid& mirrors,
                      Score<BoardParams> score = Score<BoardParams>()) :
            beam_width(beam_width),
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
        auto empty_lines = 0;
        for (auto& d : derivs) {
            empty_lines = std::max(static_cast<int>(d.params.empty_lines()),
                                   empty_lines);
        }

        auto end = derivs.begin()+std::min(derivs.size(),
                                           beam_width.width(empty_lines));
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
    BeamWidth beam_width;
    ZobristHashing hashing;
    const BoardGrid& mirrors;
    Less<Score<BoardParams>> less {};
    std::unordered_set<hash_value_t> visited;
    std::vector<BoardItem> next_boards;
    std::vector<BoardItem> boards;
    std::vector<Derivative> derivs;
};

}