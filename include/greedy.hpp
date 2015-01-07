//
//  greedy.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/28/14.
//
//

#ifndef FRAGILE_MIRRORS_greedy_hpp
#define FRAGILE_MIRRORS_greedy_hpp

#include "board_v4.hpp"

struct Greedy {
    using Board = Board_v4;

    struct Derivative {
        Derivative() {}
        Derivative(const Position& cast, Count destroyed_count)
            : cast(cast), destroyed_count(destroyed_count) {} 
    
        Position cast;
        Count destroyed_count;
    };
    
    struct Previous {
        Previous() {}
        Previous(const Position& cast, Count cast_count)
        : cast(cast), cast_count(cast_count) {} 
        
        Position cast;
        Index cast_count;
    };
    
    
    // need to pass in score function 
    using Func = function<double(const Board& b)>;
    Board Destroy(const Board& b, const Func& func) {
        Board board = b;
        while (!board.AllDestroyed()) {
            if (board.EmptySpace() > 0.5 * board.FilledSpace()) board.Reduce();
            double best_score = numeric_limits<double>::max();
            short best_ray;
            for (int ray = 0; ray < board.RayCount(); ++ray) {
                board.CastRestorable(ray);
                double score = func(board);
                if (score < best_score) {
                    best_score = score;
                    best_ray = ray;
                }
                board.Restore();
            }    
            board.Cast(best_ray);
        }
        return board;
    }

};


#endif
