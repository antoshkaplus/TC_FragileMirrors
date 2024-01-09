#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_i11 {
    using Board = Board_;
    using Value = double;

    Score_i11(board_size_t sz, double param) : param(param), sz(sz) {}

    double density(const Board& b) const {
        return static_cast<double>(sz*sz - b.destroyed_count()) / ((sz - b.empty_cols()) * (sz - b.empty_rows()));
    }

    double operator()(const Board& b) const {
        // with lower density empty lines have more value.
        return b.destroyed_count() + param * (b.empty_cols() + b.empty_rows()) * (1. - density(b));
    }

    double param;
    board_size_t sz;
};

}