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
#include "mirrors/board/board_n_i5_params.hpp"
#include "mirrors/board/board_n_i1_util.hpp"


namespace mirrors {

// From Board_n_i2. Adds tracking number of lines with even/odd number of elements.
class Board_n_i5 {
    static constexpr board_size_t kBorderSize = 2;

    using Neighbors = OriginGrid<Grid<std::array<board_size_t ,4>>>;
    using BoardGrid = OriginGrid<Grid<Mirror>>;
    using CastNode_ = CastNode<Position>;

public:
    Board_n_i5(board_size_t size) :
            neighbors_(Position{-1, -1},
                       size + kBorderSize),
            row_mirror_count(size, size),
            col_mirror_count(row_mirror_count) {
        InitNeighbors();
    }

    // Before calling it, get params from calling RestoreCast
    void Cast(const Position &pos,
              const BoardGrid& mirs) {
        cast_node = CastNode_::Push(cast_node, pos);
        auto next = NextFromBorder(pos, neighbors_[pos], size());
        while (mirs[next.pos] != Mirror::Border) {
            DestroyNeighbours(next.pos);
            --row_mirror_count[next.pos.row];
            --col_mirror_count[next.pos.col];
            // Can do that because Destroy does not touch itself neighbors_[next.pos].
            next = NextFrom(next, neighbors_[next.pos], mirs[next.pos]);
        }
    }

    Board_n_i5_Params RestoreCast(const Position &pos,
                                  Board_n_i5_Params res,
                                  const BoardGrid& mirs,
                                  const ZobristHashing& hashing) {
        restore.clear();
        // Forward
        auto next = NextFromBorder(pos, neighbors_[pos], size());
        while (mirs[next.pos] != Mirror::Border) {
            DestroyNeighbours(next.pos);
            ++res.destroyed_count_;
            hashing.xorState(&res.hash, next.pos);
            auto new_row_count = --row_mirror_count[next.pos.row];

            if (even(new_row_count)) {
                // Empty could be considered as even,
                // but for that empty param should probably be recomputed.
                if (new_row_count == 0) {
                    ++res.empty_rows_;
                } else ++res.even_rows_;
            } else --res.even_rows_;

            auto new_col_count = --col_mirror_count[next.pos.col];
            if (even(new_col_count)) {
                if (new_col_count == 0) {
                    ++res.empty_cols_;
                } else ++res.even_cols_;
            } else --res.even_cols_;

            restore.push_back(next.pos);
            // Can do that because Destroy does not touch itself neighbors_[next.pos].
            next = NextFrom(next, neighbors_[next.pos], mirs[next.pos]);
        }
        // Backward
        for (auto& r_pos: std::views::reverse(restore)) {
            RestoreNeighbours(r_pos);
            ++row_mirror_count[r_pos.row];
            ++col_mirror_count[r_pos.col];
        }
        return res;
    }

    std::vector<Position> cast_history() const {
        return CastNode_::ToHistory(cast_node);
    }

    board_size_t size() const {
        return neighbors_.grid().size()-kBorderSize;
    }

    cell_count_t cell_count() const {
        return static_cast<cell_count_t>(size())*size();
    }

    void ForEachCastCandidate(const std::function<void(const Position&)>& func) const {
        for (board_size_t i = 0; i < size(); ++i) {
            func({i, -1});
            func({i, size()});
            func({-1, i});
            func({size(), i});
        }
    }

protected:
    void InitNeighbors() {
        // Init mirrors.
        for (auto row = 0; row < size(); ++row) {
            for (auto col = 0; col < size(); ++col) {
                auto &ns = neighbors_(row, col);
                ns[kDirTop] = row - 1;
                ns[kDirBottom] = row + 1;
                ns[kDirLeft] = col - 1;
                ns[kDirRight] = col + 1;
            }
        }
        // Init borders.
        for (auto i = 0; i < size(); ++i) {
            neighbors_(-1, i)[kDirBottom] = 0;
            neighbors_(size(), i)[kDirTop] = size() - 1;
            neighbors_(i, size())[kDirLeft] = size() - 1;
            neighbors_(i, -1)[kDirRight] = 0;
        }
    }

    void DestroyNeighbours(const Position &p) {
        auto &n = neighbors_(p);
        neighbors_(n[kDirTop], p.col)[kDirBottom] = n[kDirBottom];
        neighbors_(n[kDirBottom], p.col)[kDirTop] = n[kDirTop];
        neighbors_(p.row, n[kDirLeft])[kDirRight] = n[kDirRight];
        neighbors_(p.row, n[kDirRight])[kDirLeft] = n[kDirLeft];
    }

    void RestoreNeighbours(const Position& p) {
        auto& n = neighbors_(p);
        neighbors_(n[kDirTop], p.col)[kDirBottom] = p.row;
        neighbors_(n[kDirBottom], p.col)[kDirTop] = p.row;
        neighbors_(p.row, n[kDirLeft])[kDirRight] = p.col;
        neighbors_(p.row, n[kDirRight])[kDirLeft] = p.col;
    }

    Neighbors neighbors_;
    // In Position row is first.
    std::vector<board_size_t> row_mirror_count;
    std::vector<board_size_t> col_mirror_count;
    std::shared_ptr<CastNode_> cast_node {};

    static std::vector<Position> restore;
};

}