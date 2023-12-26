#pragma once
#include <ranges>
#include <vector>
#include <string>
#include <array>
#include <memory>
#include "mirrors/common/types.hpp"
#include "mirrors/common/position.hpp"
#include "mirrors/common/origin_grid.hpp"
#include "mirrors/common/cast_node.hpp"
#include "mirrors/common/zobrist_hashing.hpp"
#include "mirrors/board/board_n_i2.hpp"
#include "mirrors/board/board_n_i2_params.hpp"
#include "mirrors/board/board_n_i1_util.hpp"


namespace mirrors {

// Adds a custom Destroy method when the last row/col cell left.
// Adds member functions to check for that scenario.
class Board_n_i3 : public Board_n_i2 {
    using CastNode_ = CastNode<Position>;

public:
    using Board_n_i2::Board_n_i2;

    void DestroyIsolated(const Position& pos, Board_n_i2_Params& params, const ZobristHashing& hashing) {
        cast_node = CastNode_::Push(cast_node, {-1, pos.col});
        DestroyNeighbours(pos);
        --row_mirror_count[pos.row];
        --col_mirror_count[pos.col];
        ++params.destroyed_count_;
        hashing.xorState(&params.hash, pos);
        ++params.empty_rows_;
        ++params.empty_cols_;
    }

    // TODO: Could become a template and better optimized.
    Position NextFromBorder(const Position& border_pos) {
        return mirrors::NextFromBorder(border_pos, neighbors_[border_pos], size()).pos;
    }

    bool single_col_mirror(board_size_t col) const {
        return col_mirror_count[col] == 1;
    }

    bool single_row_mirror(board_size_t row) const {
        return row_mirror_count[row] == 1;
    }
};

}