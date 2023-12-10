#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_i1 {
    using Board = Board_;

    std::tuple<cell_count_t, board_size_t> operator()(const Board& b) const {
        return {b.destroyed_count(), b.empty_cols() + b.empty_rows()};
    }
};

}