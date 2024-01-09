#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_Psyho {
    using Board = Board_;
    using Value = double;

    Score_Psyho(board_size_t sz) : sz(sz) {}

    double operator()(const Board& b) const {
        return b.destroyed_count()
                + static_cast<double>(b.empty_cols() + b.empty_rows()) * sz / 2;
    }

    board_size_t sz;
};

}