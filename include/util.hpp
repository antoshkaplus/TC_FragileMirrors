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
#include <array>

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

struct CastNode {
    CastNode(const Position& cast, const shared_ptr<CastNode>& previous)
    : cast{cast}, previous{previous} {}
    
    static Count Count(shared_ptr<CastNode> node) {
        ant::Count count = 0;
        while (node) {
            ++count;
            node = node->previous;
        }
        return count;
    }
    
    static vector<Position> ToHistory(shared_ptr<CastNode> node) {
        vector<Position> casts;
        while (node) {
            casts.push_back(node->cast);
            node = node->previous;
        }
        reverse(casts.begin(), casts.end());
        return casts;
    }
    
    
    Position cast;
    shared_ptr<CastNode> previous;
};

using Direction = char;




// this shit should be out of here!!

// should be somewhere in cpp probably
// from 50 to 100
constexpr array<double, 51> EMPTY_LINES_PARAM = { {
    6.396, // 50
    9.9972,
    8.33269,
    10.4997,
    7.4959,
    7.9984,
    7.9984,
    11.333,
    7.9984,
    11.9981,
    7.9984, // 60
    9.9972,
    11.9981,
    13.3318,
    10.6623,
    7.33329,
    10.9966,
    14.9963,
    9.5003,
    15.9957,
    6.33389, // 70
    10.6623,
    9.9972,
    12.4985,
    7.4959,
    10.9966,
    8.66351,
    10.4997,
    8.5009,
    9.5003,
    7.66411, // 80
    15.4988,
    10.6623,
    11.9981,
    9.9972,
    14.4994,
    11.4956,
    10.3315,
    10.6623,
    12.4985,
    13.5, // 90
    10.6623,
    12.4985,
    13.3318,
    8.24685,
    9.9972,
    12.3324,
    10.6623,
    6.4965,
    11.4956,
    11.2506 // 100
} };

constexpr array<double, 51> OVERHEAD_PARAM = { {
    1.700666, // 50
    1.971176, 
    2.153436, 
    2.074502, 
    1.445232, 
    1.199114, 
    1.882926, 
    1.445232, 
    1.445232, 
    1.946784, 
    1.612416, // 60
    2.153436, 
    1.533482, 
    1.882926, 
    1.612416, 
    1.469623, 
    1.572949, 
    1.445232, 
    2.153436, 
    1.612416, 
    1.445232, // 70
    1.445232, 
    1.803992, 
    1.700666, 
    1.740133, 
    1.533482, 
    1.341906, 
    1.238580, 
    1.469623, 
    2.153436, 
    1.700666, // 80
    1.445232, 
    1.779600, 
    1.700666, 
    1.174722, 
    1.445232, 
    1.971176, 
    1.445232, 
    1.740133, 
    2.846564, 
    1.366298, // 90
    1.700666, 
    1.700666, 
    1.612416, 
    1.700666, 
    1.676275, 
    1.262972, 
    1.882926, 
    1.700666, 
    2.153436, 
    1.700666 // 100
} };

constexpr array<double, 71 - 50 + 1> REDUCE_PARAM = { { 
    0.31952, // 50
    4.91602,
    0.23984,
    0.23984,
    0.31952,
    0.23984,
    0.31952,
    0.23984,
    0.23984,
    0.289085,
    0.289085, // 60
    0.289085,
    0.23984,
    0.23984,
    0.23984,
    0.31952,
    0.23984,
    0.23984,
    0.23984,
    0.289085,
    7.13099, // 70
    0.368765
} };  



#endif /* defined(__FRAGILE_MIRRORS__util__) */
