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
#include <iostream>
#include <vector>
#include <stack>
#include <type_traits>
#include <tuple>
#include <fstream>
#include <chrono>


#include "ant/grid/grid.hpp"

using namespace std;
using namespace ant;
using namespace ant::grid;

using StrBoard = vector<string>;

extern default_random_engine RNG;

vector<string> GenerateStringBoard(int sz);
vector<string> ReadBoard(istream& cin);
void PrintSolution(ostream& cout, const vector<Position>& sol);
void PrintSolution(ostream& cout, const vector<int>& sol);
std::vector<int> ToSolution(const std::vector<Position>& ps);
void PrintStringBoard(const vector<string>& b);
StrBoard ConvertToLetterMirrors(StrBoard b);


template<class Board>
void PrintMirrors(const Board& b) {
    auto ms = b.mirrors();
    auto func = [&](Position p) {
        cout << (int)ms[p];
        if (p.col == b.size()-1) cout << endl;
    };

    Region(0, 0, b.size(), b.size()).ForEach(func);
}

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
    
    static ant::Count Count(shared_ptr<CastNode> node) {
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




const constexpr char kMirRight     = 0;
const constexpr char kMirLeft      = 1;
const constexpr char kMirBorder    = 2;

const constexpr char kOrientHor = 0;
const constexpr char kOrientVer = 1;




#endif /* defined(__FRAGILE_MIRRORS__util__) */
