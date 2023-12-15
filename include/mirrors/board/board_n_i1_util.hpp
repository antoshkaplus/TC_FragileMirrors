#pragma once
#include <boost/format.hpp>
#include "mirrors/common/types.hpp"
#include "mirrors/common/position.hpp"


namespace mirrors {

struct Next_n {
    dir_t from_dir;
    Position pos;
};

inline Next_n NextFromBorder(const Position &p, const std::array<board_size_t ,4>& ns, board_size_t size) {
    if (p.row == -1) {
        return {kDirTop, {ns[kDirBottom], p.col}};
    } else if (p.col == -1) {
        return {kDirLeft, {p.row, ns[kDirRight]}};
    } else if (p.row == size) {
        return {kDirBottom, {ns[kDirTop], p.col}};
    } else if (p.col == size) {
        return {kDirRight, {p.row, ns[kDirLeft]}};
    } else {
        auto s = boost::format("Cant deduct direction from position %1%") % p;
        throw std::runtime_error(s.str());
    }
}

// returns next position and from which direction ray will come to that position
inline Next_n NextFrom(const Next_n& current, const std::array<board_size_t ,4>& ns, Mirror mirror) {
    auto& p = current.pos;
    switch (mirror) {
        case Mirror::Right: {
            switch (current.from_dir) {
                case kDirTop:
                    return {kDirLeft, {p.row, ns[kDirRight]}};
                case kDirBottom:
                    return {kDirRight, {p.row, ns[kDirLeft]}};
                case kDirLeft:
                    return {kDirTop, {ns[kDirBottom], p.col}};
                case kDirRight:
                    return {kDirBottom, {ns[kDirTop], p.col}};
                default:
                    assert(false);
            }
        }
        case Mirror::Left: {
            switch (current.from_dir) {
                case kDirTop:
                    return {kDirRight, {p.row, ns[kDirLeft]}};
                case kDirBottom:
                    return {kDirLeft, {p.row, ns[kDirRight]}};
                case kDirLeft:
                    return {kDirBottom, {ns[kDirTop], p.col}};
                case kDirRight:
                    return {kDirTop, {ns[kDirBottom], p.col}};
                default:
                    assert(false);
            }
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