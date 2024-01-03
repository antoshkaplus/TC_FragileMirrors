#pragma once
#include <array>
#include <boost/format.hpp>
#include "mirrors/common/types.hpp"


namespace mirrors {

struct Next_r6 {
    cell_count_t loc;
    dir_t from_dir;
};

// returns next position and from which direction ray will come to that position
inline Next_r6 NextFrom(const Next_r6& current, const std::array<cell_count_t, 4>& ns, mir_t mirror) {
    dir_t to_dir = kMirDirections[mirror][current.from_dir];
    return {ns[to_dir], kDirOpposite[to_dir]};
}

}
