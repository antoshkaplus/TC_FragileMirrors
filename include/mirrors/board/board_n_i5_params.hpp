#pragma once
#include "mirrors/common/types.hpp"


namespace mirrors {

inline bool even(board_size_t sz) {
    return sz % 2 == 0;
}


struct Board_n_i5_Params {
    hash_value_t hash {};
    cell_count_t destroyed_count_ {};
    board_size_t empty_rows_ {};
    board_size_t empty_cols_ {};
    board_size_t even_rows_ {};
    board_size_t even_cols_ {};

    Board_n_i5_Params() = default;
    Board_n_i5_Params(board_size_t size) {
        if (even(size)) {
            even_rows_ = size;
            even_cols_ = size;
        }
    }

    cell_count_t destroyed_count() const {
        return destroyed_count_;
    }
    board_size_t empty_rows() const {
        return empty_rows_;
    }
    board_size_t empty_cols() const {
        return empty_cols_;
    }
    board_size_t even_rows() const {
        return even_rows_;
    }
    board_size_t even_cols() const {
        return even_cols_;
    }

    template<class Board>
    bool board_empty(const Board& b) const {
        return destroyed_count_ == b.cell_count();
    }
};

}