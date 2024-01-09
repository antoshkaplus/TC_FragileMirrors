#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

// From Score_i4. Add number of lines with even number of mirrors left, excluding empty lines.
template <class Board_>
struct Score_i8 {
    using Board = Board_;
    using Value = double;

    double operator()(const Board& b) const {
        return b.destroyed_count() + empty_param * (b.empty_cols() + b.empty_rows()) + even_param * (b.even_cols() + b.even_rows());
    }

    double empty_param {};
    double even_param {};
};

}