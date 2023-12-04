#pragma once
#include <cstdlib>
#include <bitset>
#include <random>
#include <algorithm>
#include "common/grid.hpp"
#include "common/types.hpp"


namespace mirrors {

struct ZobristHashing {
    // will be just all zeros
    static constexpr hash_value_t NOTHING = 0;

    ZobristHashing(const board_size_t& board_size)
            : hash_table_(board_size) {
        // will use 0 for nothing
        std::uniform_int_distribution<hash_value_t> distr(1);
        std::default_random_engine rng;
        std::generate(hash_table_.begin(), hash_table_.end(),
                      [&]() {return distr(rng);});
    }

    // When you destroy a mirror call it.
    // When you place it back call it too.
    void xorState(hash_value_t* set, const Position& pos) {
        (*set) ^= hash_table_[pos];
    }

    Grid<hash_value_t> hash_table_;
};

}