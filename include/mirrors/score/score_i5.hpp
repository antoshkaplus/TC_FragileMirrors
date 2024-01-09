#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_i5 {
    using Board = Board_;
    using Value = std::tuple<cell_count_t, cell_count_t>;

    std::tuple<cell_count_t, cell_count_t> operator()(const Board& b) const {
        return { b.empty_cols() + b.empty_rows(), b.destroyed_count() };
    }
};

}