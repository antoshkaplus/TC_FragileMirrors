//
//  score.hpp
//  FragileMirrors
//
//  Created by Anton Logunov on 6/8/15.
//
//

#ifndef FragileMirrors_score_hpp
#define FragileMirrors_score_hpp

#include <array>

#include "util.hpp"

using namespace std;


template <class Board>
class Score {
public:
    double operator()(const Board& b) const {
        return b.MirrorsDestroyed();
    }
};

extern const array<double, 51> EMPTY_LINES_PARAM;

template <class Board>
class Score_v1 {
    Count N;

public:
    Score_v1(Count N) : N(N) {}

    double operator()(const Board& b) const {
        return b.MirrorsDestroyed() + EMPTY_LINES_PARAM[N-50] * b.EmptyLinesCount();
    }
};




#endif
