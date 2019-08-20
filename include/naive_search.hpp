//
//  naive_search.hpp
//  FragileMirrors
//
//  Created by Anton Logunov on 6/8/15.
//
//

#ifndef FragileMirrors_naive_search_hpp
#define FragileMirrors_naive_search_hpp


/// Score is a function or object with call operator, 
/// that receives one board argument and returns double that determines
/// how good the board is, the bigger the better 
template<class Board, class Score>
class NaiveSearch {
    
    using C = typename Board::CastType;

public:
    Board Destroy(const Board& b_in, Score& s) {
        Board bb(b_in);
        C best_cast{};
        double score, best_score = numeric_limits<double>::min();
        while (!bb.AllDestroyed()) {
            auto func = [&](auto cast) {
                score = s(bb);
                if (score > best_score) {
                    best_cast = cast;
                    best_score = score;
                }
            };
            bb.ForEachAppliedCast(func);
            bb.Cast(best_cast);
        }
        return bb;
    }
};


#endif
