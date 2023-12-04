#pragma once
#include <sstream>
#include "common/types.hpp"
#include "common/position.hpp"


namespace mirrors::board {

struct Next {
    Direction from_dir;
    Position pos;
};

Next NextFromBorder(const Position& p, board_size_t size) {
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




}