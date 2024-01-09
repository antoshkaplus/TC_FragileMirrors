#pragma once
#include <vector>
#include <array>
#include <functional>
#include <ranges>
#include "mirrors/common/types.hpp"
#include "mirrors/common/position.hpp"
#include "mirrors/common/cast_node.hpp"
#include "mirrors/common/grid.hpp"
#include "mirrors/common/origin_grid.hpp"
#include "mirrors/common/zobrist_hashing.hpp"
#include "mirrors/board/board_r6_util.hpp"
#include "mirrors/board/board_n_i5_params.hpp"


namespace mirrors {

// Based on Board_r6. Computes Even Lines.
class Board_r7 {
public:
    using CastValue = cell_count_t;
private:
    // Includes border cells with Border values in it.
    using BoardGrid = Grid<mir_t>;
    using CastNode_ = CastNode<Position>;
    using Neighbors = std::array<cell_count_t, 4>;

    static constexpr cell_count_t kBorderIndentIdx = 10000;

    struct Cell {
        Neighbors neighbours;
        Position pos;
    };

    struct Cast_ {
        Position pos;
        Next_r6 next;
    };

public:
    Board_r7(const board_size_t size) :
            cells(size*size),
            casts(4 * size),
            row_mirror_count(size, size),
            col_mirror_count(row_mirror_count) {

        destroyed = Grid<uint32_t>(size, 0);

        auto index = [=](auto r, auto c) {
            return r * size + c;
        };

        for (auto i = 0; i < size; ++i) {
            // Top
            casts[i].pos = {-1, i};
            casts[i].next = {index(0, i), kDirTop};
            // Bottom
            casts[size+i].pos = {size, i};
            casts[size+i].next = {index(size-1, i), kDirBottom};
            // Left
            casts[size+size+i].pos = {i, -1};
            casts[size+size+i].next = {index(i, 0), kDirLeft};
            // Right
            casts[size+size+size+i].pos = {i, size};
            casts[size+size+size+i].next = {index(i, size-1), kDirRight};
        }
        for (auto r = 0; r < size; ++r) {
            for (auto c = 0; c < size; ++c) {
                auto idx = index(r,c);
                cells[idx].neighbours[kDirTop] = (r != 0 ? index(r-1, c) : kBorderIndentIdx+c);
                cells[idx].neighbours[kDirBottom] = (r != size-1 ? index(r+1, c) : kBorderIndentIdx+size+c);
                cells[idx].neighbours[kDirLeft] = (c != 0 ? index(r,c-1) : kBorderIndentIdx+size+size+r);
                cells[idx].neighbours[kDirRight] = (c != size-1 ? index(r, c+1) : kBorderIndentIdx+3*size+r);
                cells[idx].pos = {r,c};
            }
        }
    }

    // Before calling it, get params from calling RestoreCast
    void Cast(const cell_count_t cast_idx,
              const BoardGrid& mirs) {
        cast_node = CastNode_::Push(cast_node, casts[cast_idx].pos);
        auto next = casts[cast_idx].next;
        while (next.loc < kBorderIndentIdx) {
            auto cur_loc = next.loc;
            next = NextFrom(next, cells[next.loc].neighbours, mirs[cells[next.loc].pos]);
            DestroyNeighbours(cells[cur_loc].neighbours);
            --row_mirror_count[cells[cur_loc].pos.row];
            --col_mirror_count[cells[cur_loc].pos.col];
            TryDestroyCasts(cur_loc);

            if (cur_loc != cells.size()-1) {
                cells[cur_loc] = cells.back();
                RestoreNeighbours(cur_loc);
                if (next.loc == cells.size()-1) {
                    next.loc = cur_loc;
                }
            }
            cells.pop_back();
        }
    }

    // Misleading method name. Do not do any restoration.
    Board_n_i5_Params RestoreCast(const cell_count_t cast_idx,
                                  Board_n_i5_Params res,
                                  const BoardGrid& mirs,
                                  const ZobristHashing& hashing) {
        ++destroyed_idx;
        auto next = casts[cast_idx].next;
        while (next.loc < kBorderIndentIdx) {
            auto& c = cells[next.loc];
            if (destroyed[c.pos] == destroyed_idx) {
                // pass through
                next.loc = c.neighbours[kDirOpposite[next.from_dir]];
                continue;
            }
            destroyed[c.pos] = destroyed_idx;

            ++res.destroyed_count_;
            hashing.xorState(&res.hash, cells[next.loc].pos);

            auto new_row_count = --row_mirror_count[c.pos.row];
            if (even(new_row_count)) {
                // Empty could be considered as even,
                // but for that empty param should probably be recomputed.
                if (new_row_count == 0) {
                    ++res.empty_rows_;
                } else ++res.even_rows_;
            } else --res.even_rows_;

            auto new_col_count = --col_mirror_count[c.pos.col];
            if (even(new_col_count)) {
                if (new_col_count == 0) {
                    ++res.empty_cols_;
                } else ++res.even_cols_;
            } else --res.even_cols_;

            destroyed_pos.push_back(c.pos);

            // Can do that because Destroy does not touch itself neighbors_[next.pos].
            next = NextFrom(next, c.neighbours, mirs[c.pos]);
        }

        for (auto pos: destroyed_pos) {
            ++row_mirror_count[pos.row];
            ++col_mirror_count[pos.col];
        }
        destroyed_pos.clear();

        return res;
    }

    void ForEachCastCandidate(auto func) const {
        for (cell_count_t i = 0; i < casts.size(); ++i) {
            func(i);
        }
    }

    std::vector<Position> cast_history() const {
        return CastNode_::ToHistory(cast_node);
    }

    cell_count_t cell_count() const {
        return row_mirror_count.size() * row_mirror_count.size();
    }

private:
    void DestroyNeighbours(const Neighbors& n) {
        if (n[kDirTop] < kBorderIndentIdx) {
            cells[n[kDirTop]].neighbours[kDirBottom] = n[kDirBottom];
        } else {
            casts[n[kDirTop]-kBorderIndentIdx].next.loc = n[kDirBottom];
        }

        if (n[kDirBottom] < kBorderIndentIdx) {
            cells[n[kDirBottom]].neighbours[kDirTop] = n[kDirTop];
        } else {
            casts[n[kDirBottom]-kBorderIndentIdx].next.loc = n[kDirTop];
        }

        if (n[kDirLeft] < kBorderIndentIdx) {
            cells[n[kDirLeft]].neighbours[kDirRight] = n[kDirRight];
        } else {
            casts[n[kDirLeft]-kBorderIndentIdx].next.loc = n[kDirRight];
        }

        if (n[kDirRight] < kBorderIndentIdx) {
            cells[n[kDirRight]].neighbours[kDirLeft] = n[kDirLeft];
        } else {
            casts[n[kDirRight]-kBorderIndentIdx].next.loc = n[kDirLeft];
        }
    }

    void RestoreNeighbours(cell_count_t idx) {
        auto& n = cells[idx].neighbours;
        if (n[kDirTop] < kBorderIndentIdx) {
            cells[n[kDirTop]].neighbours[kDirBottom] = idx;
        } else {
            casts[n[kDirTop]-kBorderIndentIdx].next.loc = idx;
        }

        if (n[kDirBottom] < kBorderIndentIdx) {
            cells[n[kDirBottom]].neighbours[kDirTop] = idx;
        } else {
            casts[n[kDirBottom]-kBorderIndentIdx].next.loc = idx;
        }

        if (n[kDirLeft] < kBorderIndentIdx) {
            cells[n[kDirLeft]].neighbours[kDirRight] = idx;
        } else {
            casts[n[kDirLeft]-kBorderIndentIdx].next.loc = idx;
        }

        if (n[kDirRight] < kBorderIndentIdx) {
            cells[n[kDirRight]].neighbours[kDirLeft] = idx;
        } else {
            casts[n[kDirRight]-kBorderIndentIdx].next.loc = idx;
        }
    }

    void TryDestroyCasts(cell_count_t destroyed_cell_idx) {
        auto loc = destroyed_cell_idx;
        auto& c = cells[loc];

        // If next is border cell, should be able to leave the loop without problems.
        if (c.neighbours[kDirTop] >= kBorderIndentIdx &&
            c.neighbours[kDirBottom] >= kBorderIndentIdx) {
            destroyed_cast.push_back(c.neighbours[kDirTop] - kBorderIndentIdx);
            destroyed_cast.push_back(c.neighbours[kDirBottom] - kBorderIndentIdx);
        }
        if (c.neighbours[kDirLeft] >= kBorderIndentIdx &&
            c.neighbours[kDirRight] >= kBorderIndentIdx) {
            destroyed_cast.push_back(c.neighbours[kDirLeft] - kBorderIndentIdx);
            destroyed_cast.push_back(c.neighbours[kDirRight] - kBorderIndentIdx);
        }
        std::sort(destroyed_cast.begin(), destroyed_cast.end(), std::greater());
        for (auto cast_loc : destroyed_cast) {
            if (cast_loc != casts.size()-1) {
                casts[cast_loc] = casts.back();
                auto n = casts[cast_loc].next;
                // `n.loc` can't point to casts.
                cells[n.loc].neighbours[n.from_dir] = cast_loc + kBorderIndentIdx;
            }
            casts.pop_back();
        }
        destroyed_cast.clear();
    }

    std::vector<Cell> cells;
    std::vector<Cast_> casts;
    std::vector<board_size_t> row_mirror_count;
    std::vector<board_size_t> col_mirror_count;
    std::shared_ptr<CastNode_> cast_node {};

    static uint32_t destroyed_idx;
    static Grid<uint32_t> destroyed;
    static std::vector<Position> destroyed_pos;
    static std::vector<cell_count_t> destroyed_cast;
};

}