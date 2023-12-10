#pragma once
#include "mirrors/board/board_i2.hpp"


namespace mirrors::board {

// Adds empty row/col counting.
class Board_i3: public Board_i2 {
public:
    explicit Board_i3(const Grid<Mirror>& mirrors_param) : Board_i2(mirrors_param),
                                                           col_mirror_count(mirrors_param.size(),
                                                                            mirrors_param.size()),
                                                           row_mirror_count(col_mirror_count) {}

    void OnMirrorDestroyed(const Position& pos) override {
        Board_i2::OnMirrorDestroyed(pos);
        if (--col_mirror_count[pos.col] == 0) {
            ++empty_cols_;
        }
        if (--row_mirror_count[pos.row] == 0) {
            ++empty_rows_;
        }
    }

    board_size_t empty_cols() const {
        return empty_cols_;
    }

    board_size_t empty_rows() const {
        return empty_rows_;
    }

protected:
    std::vector<board_size_t> col_mirror_count;
    std::vector<board_size_t> row_mirror_count;
    board_size_t empty_cols_ {};
    board_size_t empty_rows_ {};
};

}