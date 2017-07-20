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
#include "board.hpp"

using namespace std;

extern const array<double, 51> EMPTY_LINES_PARAM;


class Score {
public:
    virtual double operator()(const Board& b) const {
        return b.MirrorsDestroyed();
    }

    virtual ~Score() {}
};


class Score_v1 : public Score {
public:
    double operator()(const Board& b) const override {
        return b.MirrorsDestroyed() + EMPTY_LINES_PARAM[b.size()-50] * b.EmptyLinesCount();
    }
};

template<class Board>
class Score_Psyho {
public:
    double operator()(const Board& b) const {
        return b.MirrorsDestroyed() + (b.EmptyRowCount()+1)/(b.EmptyColCount()+1) * 8;
    }
};


class InterLevelScoreFunctor : public Score {

public:
    double operator()(const Board& b) const override {
        // may need to take into account something else
        return Score_v1()(b)/b.CastCount();
    }
};




#endif
