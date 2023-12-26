#pragma once
#include <array>
#include "mirrors/common/types.hpp"


namespace mirrors {

struct Board_n_i4_Params {
    using BorderConnCounts = std::array<cell_count_t, 5>;

    hash_value_t hash {};
    BorderConnCounts border_conn_counts;

    Board_n_i4_Params() = default;
    Board_n_i4_Params(board_size_t size) {
        border_conn_counts[0] = (size-2)*(size-2);
        border_conn_counts[1] = 4*(size-2);
        border_conn_counts[2] = 4;
        border_conn_counts[3] = 0;
        border_conn_counts[4] = 0;
    }

    template<class Board>
    bool board_empty(const Board& b) const {
        return std::accumulate(border_conn_counts.begin(),
                               border_conn_counts.end(), 0) == 0;
    }
};

}