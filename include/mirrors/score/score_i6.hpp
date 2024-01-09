#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_i6 {
    using Board = Board_;
    using Value = double;

    double operator()(const Board& b) const {
        return b.destroyed_count() + std::pow(b.empty_cols() + b.empty_rows(), param);
    }

    double param {};
};

}