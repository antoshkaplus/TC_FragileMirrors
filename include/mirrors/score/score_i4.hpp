#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_i4 {
    using Board = Board_;

    double operator()(const Board& b) const {
        return b.destroyed_count() + param * (b.empty_cols() + b.empty_rows());
    }

    double param {};
};

}