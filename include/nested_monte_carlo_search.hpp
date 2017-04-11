//
//  nested_monte_carlo_search.hpp
//  FragileMirrors
//
//  Created by Anton Logunov on 3/31/17.
//
//

#pragma once

#include "ant/core/core.hpp" 

#include "util.hpp"
#include "board.hpp"


Position CastIntToPosition(Index castInd, Count boardSz) {
    auto side = castInd / boardSz;
    auto index = castInd % boardSz;
    switch (side) {
        case 0: // top 
            return {-1, index};
        case 1: // right
            return {index, boardSz};
        case 2: // bottom
            return {boardSz, index};
        case 3: // left
            return {index, -1};
        default:
            throw runtime_error("CastIntToPosition invalid side");
    }
}


void RandomCast(Board_v1& b) {
    int index = uniform_int_distribution<>(0, 4*b.size()-1)(RNG);
    for (auto i = index; i < b.size() + index; ++i) {
        auto p = CastIntToPosition(i % b.size(), b.size());
        if (!b.IsLineEmpty(p)) {
            b.Cast(p);
            break;
        }
    }
}


// returns number of moves taken to destroy the mirrors
Count RandomPlayout(const Board_v1& b) {
    unique_ptr<Board> b_clone = b.Clone();
    Board_v1& b_play = static_cast<Board_v1&>(*b_clone);
    Count castCount = 0;
    while (!b_play.AllDestroyed()) {
        RandomCast(b_play);
        ++castCount;
    }
    return castCount;
}


struct NestedMonteCarloSearch {
public:

    unique_ptr<Board_v1> Destroy(const Board_v1& board) {
        auto b_clone = board.Clone();
        auto& b = static_cast<Board_v1&>(*b_clone);

        while (!b.AllDestroyed()) {
            Position bestCast;
            int minCount = numeric_limits<int>::max();
            for (auto p : b.CastCandidates()) {
                if (b.Cast(p) > 0) {
                    Count castCount = RandomPlayout(b);
                    if (minCount > castCount) {
                        minCount = castCount;
                        bestCast = p;
                    }
                }
                b.Restore();
            }
            b.Cast(bestCast);
        }

        unique_ptr<Board_v1> b_uni(&b);
        b_clone.release();
        return b_uni;
    }

};


struct MonteCarloScore {

    double operator()(const Board_v1& b) const {
        return -RandomPlayout(b);
    }

};