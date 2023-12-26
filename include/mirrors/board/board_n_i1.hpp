#pragma once
#include <vector>
#include <string>
#include <array>
#include <memory>
#include "mirrors/common/types.hpp"
#include "mirrors/common/position.hpp"
#include "mirrors/common/origin_grid.hpp"
#include "mirrors/common/cast_node.hpp"
#include "mirrors/common/zobrist_hashing.hpp"
#include "mirrors/board/board_n_i1_util.hpp"


namespace mirrors {

// This representation is similar to Board_i1 in a sense how grid is used.
// But in each cell it stores next non-destroyed mirror location above, below,
// left and right of itself. Keep a shared pointer to a board with mirrors.
class Board_n_i1 {
    static constexpr board_size_t kBorderSize = 2;

    using Neighbors = OriginGrid<Grid<std::array<board_size_t ,4>>>;
    using BoardGrid = OriginGrid<Grid<Mirror>>;
    using CastNode_ = CastNode<Position>;

public:
    Board_n_i1(const Grid<Mirror>& mirrors_param) :
            hashing_(std::make_shared<ZobristHashing>(mirrors_param.size())),
            mirrors_(std::make_shared<BoardGrid>(Position{-1, -1},
                                                 mirrors_param.size() + kBorderSize)),
            neighbors_(Position{-1, -1},
                       mirrors_param.size() + kBorderSize) {
        // Init Mirrors
        auto& mir = *mirrors_;
        mir.grid().fill(Mirror::Border);
        for (auto row = 0; row < mirrors_param.size(); ++row) {
            for (auto col = 0; col < mirrors_param.size(); ++col) {
                mir[{row, col}] = mirrors_param(row, col);
            }
        }
        InitNeighbors();
    }

    cell_count_t Cast(const Position &pos) {
        cast_node = CastNode_::Push(cast_node, pos);
        auto next = NextFromBorder(pos, neighbors_[pos], size());
        auto &mirs = *mirrors_;
        while (mirs[next.pos] != Mirror::Border) {
            Destroy(next.pos);
            ++destroyed_count_;
            OnMirrorDestroyed(next.pos);
            // Can do that because Destroy does not touch itself neighbors_[next.pos].
            next = NextFrom(next, neighbors_[next.pos], mirs[next.pos]);
        }
        return destroyed_count_;
    }

    virtual void OnMirrorDestroyed(const Position& pos) {}

    hash_value_t hash() const {
        return hash_;
    }

    bool AllDestroyed() const {
        return destroyed_count_ == static_cast<cell_count_t>(size())*size();
    }

    std::vector<Position> cast_history() const {
        return CastNode_::ToHistory(cast_node);
    }

    board_size_t size() const {
        return neighbors_.grid().size()-kBorderSize;
    }

    cell_count_t destroyed_count() const {
        return destroyed_count_;
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

    void Destroy(const Position &p) {
        auto &n = neighbors_(p);
        neighbors_(n[kDirTop], p.col)[kDirBottom] = n[kDirBottom];
        neighbors_(n[kDirBottom], p.col)[kDirTop] = n[kDirTop];
        neighbors_(p.row, n[kDirLeft])[kDirRight] = n[kDirRight];
        neighbors_(p.row, n[kDirRight])[kDirLeft] = n[kDirLeft];
        hashing_->xorState(&hash_, p);
    }

    cell_count_t destroyed_count_ {};
    hash_value_t hash_ = ZobristHashing::NOTHING;
    std::shared_ptr<ZobristHashing> hashing_;
    std::shared_ptr<CastNode_> cast_node {};

    std::shared_ptr<BoardGrid> mirrors_;
    Neighbors neighbors_;
};

}