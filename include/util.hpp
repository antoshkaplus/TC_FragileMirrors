//
//  util.h
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/25/14.
//
//

#ifndef __FRAGILE_MIRRORS__util__
#define __FRAGILE_MIRRORS__util__

#include <stdio.h>
#include <vector>
#include <string>
#include <random>

#include "ant/grid.h"

using namespace std;
using namespace ant;
using namespace ant::grid;


extern default_random_engine RNG;

vector<string> GenerateStringBoard(int sz);

constexpr bool IsRightMirror(char mirror) {
    return mirror == 'R'; // '\'
}

constexpr bool IsLeftMirror(char mirror) {
    return mirror == 'L'; // '/'
}

struct CastIterator {
    CastIterator(Count sz) : cur(0), sz(sz) {}
    
    bool HasNext() {
        return cur != 4*sz;
    }
    
    
    Position Next() {
        Index i = cur % 4;
        Index j = cur / 4;
        ++cur;
        switch (i) {
        case 0: return {j, -1};
        case 1: return {-1, j};
        case 2: return {sz, j};
        case 3: return {j, sz};
        }
        assert(false);
        return {};
    }
    
    Index cur;
    Count sz;
};


#endif /* defined(__FRAGILE_MIRRORS__util__) */
