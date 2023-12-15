#pragma once
#include "mirrors/board/board_i5.hpp"


namespace mirrors {

struct Cast {
    Cast(Board_i5 &board, const Position& pos) : board(board) {

        board.cast_node = Board_i5::CastNode_::Push(board.cast_node, pos);
        auto next = NextFromBorder(pos, board.size());
        auto& mir = board.mirrors;
        while (mir[next.pos] != Mirror::Border) {
            auto new_next = NextFrom(next, mir[next.pos]);
            if (mir[next.pos] != Mirror::Destroyed) {
                mir[next.pos] = Mirror::Destroyed;
                ++board.destroyed_count_;
                OnMirrorDestroyed(next.pos);
            }
            next = new_next;
        }
    }
private:
    void OnMirrorDestroyed(const Position& pos) {
        board.hashing_->xorState(&board.hash_, pos);
        if (--board.col_mirror_count[pos.col] == 0) {
            ++board.empty_cols_;
        }
        if (--board.row_mirror_count[pos.row] == 0) {
            ++board.empty_rows_;
        }
    }

    Board_i5 &board;
};

}