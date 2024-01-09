#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

// From Score_i4
template <class Board_>
struct Score_i7 {
    using Board = Board_;
    using Value = double;

    Score_i7(double param, double mid_bias, board_size_t board_size) : board_size(board_size) {
        auto max_value = param * 2 * board_size;
        auto mid_value = param * board_size;

        auto new_mid_value = mid_bias + mid_value;
        param_1 = new_mid_value / board_size;
        param_2 = (max_value - new_mid_value) / board_size;
    }

    double operator()(const Board& b) const {
        auto lines = b.empty_cols() + b.empty_rows();
        return b.destroyed_count()
                + param_1 * std::min(static_cast<int>(board_size), lines)
                + param_2 * std::max(0, lines-board_size);
    }

private:
    double param_1;
    // negative - bias down, upward otherwise.
    double param_2;
    board_size_t board_size;
};

}