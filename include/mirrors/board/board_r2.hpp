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
#include "mirrors/board/board_r1_util.hpp"
#include "mirrors/board/board_n_i2_params.hpp"


namespace mirrors {

// Based on Board_r. Shrink cells with destruction of mirrors.
class Board_r2 {
public:
    using CastValue = cell_count_t;
private:
    // Includes border cells with Border values in it.
    using BoardGrid = OriginGrid<Grid<Mirror>>;
    using CastNode_ = CastNode<Position>;
    using Neighbors = std::array<cell_count_t, 4>;

    static constexpr cell_count_t kBorderIndentIdx = 10000;

    struct Cell {
        Neighbors neighbours;
        Position pos;
    };

    struct Cast_ {
        Position pos;
        Next_r next;
    };

public:
    Board_r2(const board_size_t size) :
            casts(4 * size),
            cells(size*size) {

        auto index = [=](auto r, auto c) {
            return r * size + c;
        };

        for (auto i = 0; i < size; ++i) {
            // Top
            casts[i].pos = {-1, i};
            casts[i].next = {kDirTop, index(0, i)};
            // Bottom
            casts[size+i].pos = {size, i};
            casts[size+i].next = {kDirBottom, index(size-1, i)};
            // Left
            casts[size+size+i].pos = {i, -1};
            casts[size+size+i].next = {kDirLeft, index(i, 0)};
            // Right
            casts[size+size+size+i].pos = {i, size};
            casts[size+size+size+i].next = {kDirRight, index(i, size-1)};
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

    Board_n_i2_Params RestoreCast(const cell_count_t cast_idx,
                                  Board_n_i2_Params res,
                                  const BoardGrid& mirs,
                                  const ZobristHashing& hashing) {
        restore.clear();
        // Forward
        auto next = casts[cast_idx].next;
        while (next.loc < kBorderIndentIdx) {
            DestroyNeighbours(cells[next.loc].neighbours);
            ++res.destroyed_count_;
            hashing.xorState(&res.hash, cells[next.loc].pos);
            if (cells[next.loc].neighbours[kDirTop] >= kBorderIndentIdx &&
                cells[next.loc].neighbours[kDirBottom] >= kBorderIndentIdx) {
                ++res.empty_cols_;
            }
            if (cells[next.loc].neighbours[kDirLeft] >= kBorderIndentIdx &&
                cells[next.loc].neighbours[kDirRight] >= kBorderIndentIdx) {
                ++res.empty_rows_;
            }
            restore.push_back(next.loc);
            // Can do that because Destroy does not touch itself neighbors_[next.pos].
            next = NextFrom(next, cells[next.loc].neighbours, mirs[cells[next.loc].pos]);
        }
        // Backward
        for (auto& loc: std::views::reverse(restore)) {
            RestoreNeighbours(loc);
        }
        return res;
    }

    void ForEachCastCandidate(const std::function<void(const cell_count_t)>& func) const {
        for (cell_count_t i = 0; i < casts.size(); ++i) {
            func(i);
        }
    }

    std::vector<Position> cast_history() const {
        return CastNode_::ToHistory(cast_node);
    }

    cell_count_t cell_count() const {
        auto size = casts.size() / 4;
        return size * size;
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

    std::vector<Cast_> casts;
    std::vector<Cell> cells;
    std::shared_ptr<CastNode_> cast_node {};

    static std::vector<cell_count_t> restore;
};

}