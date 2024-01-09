#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_c_i1 {
    using Board = Board_;
    using Value = double;

    // The objective is minimization function,
    // so we use minus sign to make it maximization.
    double operator()(const Board& b) const {
        return -(b.border_conn_counts[0] +
                 b.border_conn_counts[1] +
                 b.border_conn_counts[2] +
                 b.border_conn_counts[3] +
                 b.border_conn_counts[4]);
    }
};

}