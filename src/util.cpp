//
//  util.cpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/25/14.
//
//

#include <random>

#include "util.hpp"

#ifdef DEBUG

default_random_engine RNG;

#else

default_random_engine RNG(std::chrono::system_clock::now().time_since_epoch().count());

#endif

vector<string> GenerateStringBoard(int sz) {
    discrete_distribution<int> distr{0.5, 0.5};
    vector<string> strBoard(sz);
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < sz; j++) {
            strBoard[i].push_back(distr(RNG) == 0 ? 'R' : 'L');
        }
    }
    return strBoard;
}


