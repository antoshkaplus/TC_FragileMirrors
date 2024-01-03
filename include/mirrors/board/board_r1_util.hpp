#pragma once
#include <array>
#include <boost/format.hpp>
#include "mirrors/common/types.hpp"


namespace mirrors {

struct Next_r {
    dir_t from_dir;
    cell_count_t loc;
};

// returns next position and from which direction ray will come to that position
inline Next_r NextFrom(const Next_r& current, const std::array<cell_count_t, 4>& ns, Mirror mirror) {
    switch (mirror) {
        case Mirror::Right: {
            switch (current.from_dir) {
                case kDirTop:
                    return {kDirLeft, ns[kDirRight]};
                case kDirBottom:
                    return {kDirRight, ns[kDirLeft]};
                case kDirLeft:
                    return {kDirTop, ns[kDirBottom]};
                case kDirRight:
                    return {kDirBottom, ns[kDirTop]};
                default:
                    assert(false);
            }
        }
        case Mirror::Left: {
            switch (current.from_dir) {
                case kDirTop:
                    return {kDirRight, ns[kDirLeft]};
                case kDirBottom:
                    return {kDirLeft, ns[kDirRight]};
                case kDirLeft:
                    return {kDirBottom, ns[kDirTop]};
                case kDirRight:
                    return {kDirTop, ns[kDirBottom]};
                default:
                    assert(false);
            }
        }
        case Mirror::None:
        case Mirror::Border:
        default: {
            auto s = boost::format("Unexpected mirror %1% at %2%") % mirror % current.loc;
            throw std::runtime_error(s.str());
        }
    }
}

}