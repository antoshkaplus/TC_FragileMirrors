#pragma once
#include <sstream>
#include <boost/format.hpp>
#include "mirrors/common/types.hpp"
#include "mirrors/common/position.hpp"


namespace mirrors {

struct Next {
    Direction from_dir;
    Position pos;
};

inline Next NextFromBorder(const Position& p, board_size_t size) {
    if (p.row == -1) {
        return {Direction::Up, {0, p.col}};
    } else if (p.col == -1) {
        return {Direction::Left, {p.row, 0}};
    } else if (p.row == size) {
        return {Direction::Down, {size-1, p.col}};
    } else if (p.col == size){
        return {Direction::Right, {p.row, size-1}};
    } else {
        std::stringstream s;
        s << "cant deduct direction from position ";
        s << p;
        throw std::runtime_error(s.str());
    }
}

// Returns next position and from which direction ray will come to that position.
inline Next NextFrom(const Next& current, Mirror mirror) {
    switch (mirror) {
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
            auto s = boost::format("Unexpected mirror %1% at %2%") % mirror % current.pos;
            throw std::runtime_error(s.str());
        }
    }
}

}