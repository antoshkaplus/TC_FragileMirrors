//
//  beam_search.hpp
//  FragileMirrors
//
//  Created by Anton Logunov on 6/8/15.
//
//

#ifndef FragileMirrors_beam_search_hpp
#define FragileMirrors_beam_search_hpp

#include "util.hpp"


template<class Board, class Score>
class BeamSearch {
    
    using HashType = typename Board::HashType;
    using CastType = typename Board::CastType;
    
    /// can make use of more parametered possibly 
    struct Derivative {
        Derivative() {}
        Derivative(Board* b, const CastType& p, HashType h, double s) 
        : origin(b), cast(p), hash(h), score(s) {}
        
        Board* origin;
        CastType cast;
        HashType hash;
        double score;
        
        /// want to sort in reverse order
        bool operator<(const Derivative& d) const {
            return score > d.score;
        }
    };
    
    Count beam_width_;
    
public:

    Board Destroy(const Board& b_in, Score& score) {
        unordered_set<HashType> visited;
        vector<Derivative> derivs;
        vector<Board> b_0;
        vector<Board> b_1;
        b_0.reserve(beam_width_);
        b_1.reserve(beam_width_);
        Count side_count = 4;
        derivs.reserve(beam_width_*side_count*b_in.size());
        auto cur = &b_0;
        auto next = &b_1; 
        cur->push_back(b_in);
        while (true) {
            for (auto& b : *cur) {
                Count d_was = b.MirrorsDestroyed();
                for (auto& c : b.CastCandidates()) {
                    b.Cast(c);
                    Count d_now = b.MirrorsDestroyed();
                    if (d_now > d_was && visited.count(b.hash()) == 0) {
                        visited.insert(b.hash());
                        derivs.emplace_back(&b, c, b.hash(), score(b));
                    }
                    b.Restore();
                }
            }
            Count sz = min<Count>(beam_width_, derivs.size());
            nth_element(derivs.begin(), derivs.begin()+sz-1, derivs.end());
            derivs.resize(sz);
            next->resize(sz);
            for (Index i = 0; i < sz; ++i) {
                derivs[i].origin->Cast(derivs[i].cast);
                (*next)[i] = *(derivs[i].origin);
                derivs[i].origin->Restore();
            }
            swap(cur, next);
            auto rr = max_element(cur->begin(), cur->end(), [] (const Board& b_0, const Board& b_1) {
                return b_0.MirrorsDestroyed() < b_1.MirrorsDestroyed();
            });
            if (rr->AllDestroyed()) {
                return *rr;   
            }
            /// cleanup before next step
            next->clear();
            derivs.clear();
            visited.clear();
        }
    }

    void set_beam_width(Count beam_width) {
        beam_width_ = beam_width;
    }
};


#endif
