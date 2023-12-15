#pragma once
#include <ranges>
#include "mirrors/board/board_i5.hpp"


namespace mirrors {

struct Restore {
    Position pos;
    Mirror mirror;
};


struct RestoreCast {
    RestoreCast(Board_i5 &board, const Position& pos) :
            board(board),
            old_hash_(board.hash()),
            old_empty_cols_(board.empty_cols()),
            old_empty_rows_(board.empty_rows()) {

        // TODO: try to reduce it
        board.cast_node = Board_i5::CastNode_::Push(board.cast_node, pos);
        restore.clear();
        auto next = NextFromBorder(pos, board.size());
        auto& mir = board.mirrors;
        while (mir[next.pos] != Mirror::Border) {
            auto new_next = NextFrom(next, mir[next.pos]);
            if (mir[next.pos] != Mirror::Destroyed) {
                restore.emplace_back(next.pos, mir[next.pos]);
                mir[next.pos] = Mirror::Destroyed;
                ++board.destroyed_count_;
                OnMirrorDestroyed(next.pos);
            }
            next = new_next;
        }
    }

    void OnMirrorDestroyed(const Position& pos) {
        board.hashing_->xorState(&board.hash_, pos);
        if (--board.col_mirror_count[pos.col] == 0) {
            ++board.empty_cols_;
        }
        if (--board.row_mirror_count[pos.row] == 0) {
            ++board.empty_rows_;
        }
    }

    ~RestoreCast() {
        for (const auto& r: restore) {
            board.mirrors[r.pos] = r.mirror;
            ++board.col_mirror_count[r.pos.col];
            ++board.col_mirror_count[r.pos.row];
            --board.destroyed_count_;
        }
        board.cast_node = board.cast_node->previous;
        board.hash_ = old_hash_;
        board.empty_cols_ = old_empty_cols_;
        board.empty_rows_ = old_empty_rows_;
    }

    Board_i5& board;
    hash_value_t old_hash_;
    board_size_t old_empty_cols_;
    board_size_t old_empty_rows_;
    static std::vector<Restore> restore;
};

}