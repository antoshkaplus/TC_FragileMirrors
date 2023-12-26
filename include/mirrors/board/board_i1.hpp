#pragma once
#include <functional>
#include <boost/format.hpp>
#include "mirrors/common/position.hpp"
#include "mirrors/common/grid.hpp"
#include "mirrors/common/origin_grid.hpp"
#include "mirrors/common/types.hpp"
#include "mirrors/board/board_i1_util.hpp"


namespace mirrors::board {

/*
 * Native board representation as a grid of `mirror` cells with
 * `border` cells on the edges and `destroyed` cells once mirror is destroyed.
 */
class Board_i1 {
    static constexpr board_size_t kBorderSize = 2;

public:
    explicit Board_i1(const Grid<Mirror>& mirrors_param) {
        mirrors = OriginGrid<Grid<Mirror>>{{-1, -1},
                                           mirrors_param.size() + kBorderSize};
        mirrors.grid().fill(Mirror::Border);
        for (auto row = 0; row < mirrors_param.size(); ++row) {
            for (auto col = 0; col < mirrors_param.size(); ++col) {
                mirrors[{row, col}] = mirrors_param(row, col);
            }
        }
    }

    // returns total mirrors destroyed.
    short Cast(const Position& pos) {
        OnCast(pos);
        auto next = NextFromBorder(pos, size());
        while (mirrors[next.pos] != Mirror::Border) {
            auto new_next = NextFrom(next, mirrors[next.pos]);
            if (mirrors[next.pos] != Mirror::Destroyed) {
                mirrors[next.pos] = Mirror::Destroyed;
                ++destroyed_count_;
                OnMirrorDestroyed(next.pos);
            }
            next = new_next;
        }
        return destroyed_count_;
    }

    virtual void OnCast(const Position& pos) {}

    virtual void OnMirrorDestroyed(const Position& pos) {}

    bool AllDestroyed() const {
        return destroyed_count_ == static_cast<cell_count_t>(size())*size();
    }

    void ForEachCastCandidate(const std::function<void(const Position&)>& func) const {
        for (board_size_t i = 0; i < size(); ++i) {
            func({i, -1});
            func({i, size()});
            func({-1, i});
            func({size(), i});
        }
    }

    board_size_t size() const {
        return mirrors.grid().size()-kBorderSize;
    }

    cell_count_t destroyed_count() const {
        return destroyed_count_;
    }

private:
    cell_count_t destroyed_count_ {};
    OriginGrid<Grid<Mirror>> mirrors;
};

}