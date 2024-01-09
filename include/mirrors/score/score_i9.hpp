#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_i9 {
    using Board = Board_;
    using Value = double;

    double operator()(const Board& b) const {
        return b.destroyed_count() + param * (b.empty_cols() * b.empty_rows());
    }

    double param {};
};

}