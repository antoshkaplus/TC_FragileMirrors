//
//  stats.cpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/27/14.
//
//

#include <stdio.h>

#include "_stats.hpp"

// coeff for each cast to get global score
vector<double> ReadDestroyedCoeffs(istream& in) {
    vector<double> result;
    Index cur_cast;
    Count exs_count;
    Count cast_count;
    Count lines_empty;
    Count destroyed_count;
    in >> cast_count;
    result.resize(cast_count);
    for (auto i = 0; i < cast_count; ++i) {
        in >> cur_cast >> exs_count;
        if (i != cur_cast) assert(false);
        double mean = 0;
        for (auto k = 0; k < exs_count; ++k) {
            in >> destroyed_count >> lines_empty;
            mean += destroyed_count;
        }
        result[i] = 1./(1.*mean/exs_count);
    }
    return result;
}
