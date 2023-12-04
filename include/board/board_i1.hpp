#pragma once
#include <functional>
#include <boost/format.hpp>
#include "common/position.hpp"
#include "common/grid.hpp"
#include "common/origin_grid.hpp"
#include "common/types.hpp"
#include "board/board_i1_util.hpp"


namespace mirrors::board {

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
            auto new_next = NextFrom(next);
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

    void ForEachCastCandidate(std::function<void(const Position&)> func) const {
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
    // returns next position and from which direction ray will come to that position
    Next NextFrom(const Next& current) {
        switch (mirrors[current.pos]) {
            case Mirror::Right: {   // '\'
                switch (current.from_dir) {
                    case Direction::Up: return {
                        Direction::Left,
                        current.pos + Indent{0, 1}
                    };
                    case Direction::Down: return {
                        Direction::Right,
                        current.pos + Indent{0, -1}
                    };
                    case Direction::Left: return {
                        Direction::Up,
                        current.pos + Indent{1, 0}
                    };
                    case Direction::Right: return {
                        Direction::Down,
                        current.pos + Indent{-1, 0}
                    };
                    default: throw std::runtime_error("Unexpected direction.");
                }
            }
            case Mirror::Left: {    // '/'
                switch (current.from_dir) {
                    case Direction::Up: return {
                        Direction::Right,
                        current.pos + Indent{0, -1}
                    };
                    case Direction::Down: return {
                        Direction::Left,
                        current.pos + Indent{0, 1}
                    };
                    case Direction::Left: return {
                        Direction::Down,
                        current.pos + Indent{-1, 0}
                    };
                    case Direction::Right: return {
                        Direction::Up,
                        current.pos + Indent{1, 0}
                    };
                    default: throw std::runtime_error("Unexpected direction.");
                }
            }
            case Mirror::Destroyed: {
                auto p = current.pos;
                switch (current.from_dir) {
                    case Direction::Up: {
                        ++p.row;
                        break;
                    }
                    case Direction::Down: {
                        --p.row;
                        break;
                    }
                    case Direction::Left: {
                        ++p.col;
                        break;
                    }
                    case Direction::Right: {
                        --p.col;
                        break;
                    }
                    default: throw std::runtime_error("Unexpected direction.");
                }
                return {current.from_dir, p};
            }
            case Mirror::None:
            case Mirror::Border:
            default: {
                auto s = boost::format("Unexpected mirror %1% at %2%") % mirrors[current.pos] % current.pos;
                throw std::runtime_error(s.str());
            }
        }
    }

    cell_count_t destroyed_count_ {};
    OriginGrid<Grid<Mirror>> mirrors;
};

}