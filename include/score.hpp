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

// better call functor for clarity
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

public:

    double operator()(const Board& b) const {
        return b.MirrorsDestroyed() + EMPTY_LINES_PARAM[b.size()-50] * b.EmptyLinesCount();
    }
};

template <class Board>
class InterLevelScoreFunctor {

public:
    double operator()(const Board& b) const {
        // may need to take into account something else
        return Score_v1<Board>()(b)/b.CastCount();
    }

};




#endif
