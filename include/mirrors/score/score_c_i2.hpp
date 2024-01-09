#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_c_i2 {
    using Board = Board_;
    using Value = double;
    using Param = std::array<float, 4>;

    Score_c_i2() = default;
    Score_c_i2(const Param& param) : param(param) {}

    // The objective is minimization function,
    // so we use minus sign to make it maximization.
    double operator()(const Board& b) const {
        return -(b.border_conn_counts[0] +
                 param[0] * b.border_conn_counts[1] +
                 param[1] * b.border_conn_counts[2] +
                 param[2] * b.border_conn_counts[3] +
                 param[3] * b.border_conn_counts[4]);
    }

    Param param {};
};

}