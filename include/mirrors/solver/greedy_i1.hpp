#pragma once
#include <vector>
#include "mirrors/board/board_i1.hpp"


namespace mirrors::solver {

// Greedy solver: each level take board with most destroyed mirrors.
struct Greedy_i1 {
    std::vector<Position> Destroy(const board::Board_i1& b) {
        std::vector<Position> cast_history;
        auto board = b;
        while (!board.AllDestroyed()) {
            Position max_destroyed_cast;
            auto max_destroyed = 0;
            board.ForEachCastCandidate([&](const Position& p) {
                auto cast_board = board;
                auto destroyed = cast_board.Cast(p);
                if (destroyed > max_destroyed) {
                    max_destroyed = destroyed;
                    max_destroyed_cast = p;
                }
            });
            board.Cast(max_destroyed_cast);
            cast_history.push_back(max_destroyed_cast);
        }
        return cast_history;
    }
};

}