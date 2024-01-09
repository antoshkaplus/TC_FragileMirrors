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
#include "mirrors/board/board_n_i4_params.hpp"
#include "mirrors/board/board_n_i1_util.hpp"


namespace mirrors {

// From Board_n_i2. Instead of keeping track of destroyed mirrors and number of empty lines,
// it tracks number of mirrors per border connections count.
class Board_n_i4 {
    static constexpr board_size_t kBorderSize = 2;

    using Neighbors = OriginGrid<Grid<std::array<board_size_t ,4>>>;
    using BoardGrid = OriginGrid<Grid<Mirror>>;
    using CastNode_ = CastNode<Position>;
    using BorderConnCounts = std::array<cell_count_t, 5>;
    using BoardConnCounts = Grid<board_size_t>;
public:
    using CastValue = Position;

    Board_n_i4(board_size_t size) :
            neighbors_(Position{-1, -1},
                       size + kBorderSize),
            board_conn_counts_(size, 0) {
        InitNeighbors();
        InitBorderConn();
    }

    // Before calling it, get params from calling RestoreCast
    void Cast(const Position &pos,
              const BoardGrid& mirs) {
        cast_node = CastNode_::Push(cast_node, pos);
        auto next = NextFromBorder(pos, neighbors_[pos], size());
        while (mirs[next.pos] != Mirror::Border) {
            DestroyNeighbours(next.pos);
            Board_n_i4_Params params;
            AdjustBorderConnOnDestroyed(next.pos, params);
            // Can do that because Destroy does not touch itself neighbors_[next.pos].
            next = NextFrom(next, neighbors_[next.pos], mirs[next.pos]);
        }
    }

    Board_n_i4_Params RestoreCast(const Position &pos,
                      Board_n_i4_Params params,
                      const BoardGrid& mirs,
                      const ZobristHashing& hashing) {
        restore.clear();
        // Forward
        auto next = NextFromBorder(pos, neighbors_[pos], size());
        while (mirs[next.pos] != Mirror::Border) {
            DestroyNeighbours(next.pos);
            AdjustBorderConnOnDestroyed(next.pos, params);
            hashing.xorState(&params.hash, next.pos);
            restore.push_back(next.pos);
            // Can do that because Destroy does not touch itself neighbors_[next.pos].
            next = NextFrom(next, neighbors_[next.pos], mirs[next.pos]);
        }
        // Backward
        for (auto& r_pos: std::views::reverse(restore)) {
            RestoreNeighbours(r_pos);
            AdjustBorderConnOnRestored(r_pos);
        }
        return params;
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

    void InitBorderConn() {
        for (auto i = 0; i < size(); ++i) {
            ++board_conn_counts_(0, i);
            ++board_conn_counts_(size()-1, i);
            ++board_conn_counts_(i, 0);
            ++board_conn_counts_(i, size()-1);
        }
    }

    // Used on Cast
    void IncBorderConn(const Position& pos, Board_n_i4_Params& params) {
        --params.border_conn_counts[board_conn_counts_[pos]];
        ++board_conn_counts_[pos];
        ++params.border_conn_counts[board_conn_counts_[pos]];
    }

    // Used on Restore
    void DecBorderConn(const Position& pos) {
        --board_conn_counts_[pos];
    }

    void DestroyNeighbours(const Position &p) {
        auto &n = neighbors_(p);
        neighbors_(n[kDirTop], p.col)[kDirBottom] = n[kDirBottom];
        neighbors_(n[kDirBottom], p.col)[kDirTop] = n[kDirTop];
        neighbors_(p.row, n[kDirLeft])[kDirRight] = n[kDirRight];
        neighbors_(p.row, n[kDirRight])[kDirLeft] = n[kDirLeft];
    }

    void AdjustBorderConnOnDestroyed(const Position& p, Board_n_i4_Params& params) {
        auto &n = neighbors_(p);
        --params.border_conn_counts[board_conn_counts_[p]];

        if (n[kDirTop] >= 0) {
            if (n[kDirBottom] < size()) {} // nothing changes in conn counts
            else {
                IncBorderConn({n[kDirTop], p.col}, params);
            }
        }
        if (n[kDirBottom] < size()) {
            if (n[kDirTop] >= 0) {}
            else {
                IncBorderConn({n[kDirBottom], p.col}, params);
            }
        }
        if (n[kDirLeft] >= 0) {
            if (n[kDirRight] < size()) {}
            else {
                IncBorderConn({p.row, n[kDirLeft]}, params);
            }
        }
        if (n[kDirRight] < size()) {
            if (n[kDirLeft] >= 0) {}
            else {
                IncBorderConn({p.row, n[kDirRight]}, params);
            }
        }
    }

    void RestoreNeighbours(const Position& p) {
        auto& n = neighbors_(p);
        neighbors_(n[kDirTop], p.col)[kDirBottom] = p.row;
        neighbors_(n[kDirBottom], p.col)[kDirTop] = p.row;
        neighbors_(p.row, n[kDirLeft])[kDirRight] = p.col;
        neighbors_(p.row, n[kDirRight])[kDirLeft] = p.col;
    }

    void AdjustBorderConnOnRestored(const Position& p) {
        auto &n = neighbors_(p);
        if (n[kDirTop] >= 0) {
            if (n[kDirBottom] < size()) {} // nothing changes in conn counts
            else {
                DecBorderConn({n[kDirTop], p.col});
            }
        }
        if (n[kDirBottom] < size()) {
            if (n[kDirTop] >= 0) {}
            else {
                DecBorderConn({n[kDirBottom], p.col});
            }
        }
        if (n[kDirLeft] >= 0) {
            if (n[kDirRight] < size()) {}
            else {
                DecBorderConn({p.row, n[kDirLeft]});
            }
        }
        if (n[kDirRight] < size()) {
            if (n[kDirLeft] >= 0) {}
            else {
                DecBorderConn({p.row, n[kDirRight]});
            }
        }
    }

    Neighbors neighbors_;
    BoardConnCounts board_conn_counts_;
    std::shared_ptr<CastNode_> cast_node {};
    static std::vector<Position> restore;
};

}