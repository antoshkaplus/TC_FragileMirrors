#pragma once
#include "mirrors/board/board_i3.hpp"


namespace mirrors::board {

// Skips empty lines during candidate cast traversal.
class Board_i4 : public Board_i3 {
public:
    using Board_i3::Board_i3;

    void ForEachCastCandidate(const std::function<void(const Position &)>& func) const {
        for (board_size_t i = 0; i < size(); ++i) {
            if (row_mirror_count[i] != 0) {
                func({i, -1});
                func({i, size()});
            }
            if (col_mirror_count[i] != 0) {
                func({-1, i});
                func({size(), i});
            }
        }
    }
};

}