#pragma once
#include <iostream>
#include "common/types.hpp"

namespace mirrors {

struct Position {
    board_size_t row;
    board_size_t col;
};

struct Indent {
    board_size_t row;
    board_size_t col;
};

inline Position operator+(const Position& pos, const Indent& indent) {
    return {pos.row + indent.row, pos.col + indent.col};
}
inline Position& operator+=(Position& pos, const Indent& indent) {
    pos.row += indent.row;
    pos.col += indent.col;
    return pos;
}
inline std::ostream& operator<<(std::ostream& out, const Position& p) {
    return out << static_cast<int>(p.row) << " " << static_cast<int>(p.col);
}

}