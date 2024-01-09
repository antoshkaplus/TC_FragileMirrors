#pragma once
#include <tuple>
#include <mirrors/common/types.hpp>

namespace mirrors {

template <class Board_>
struct Score_i3 {
    using Board = Board_;
    using Value = std::tuple<cell_count_t, cell_count_t, cell_count_t>;

    Value operator()(const Board& b) const {
        return {b.destroyed_count(), b.empty_cols() + b.empty_rows(), b.empty_cols()*b.empty_cols() +
                                                                      b.empty_rows()*b.empty_rows()};
    }
};

}