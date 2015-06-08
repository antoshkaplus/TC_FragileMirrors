//
//  best_first_search.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/28/14.
//
//

#ifndef FRAGILE_MIRRORS_best_first_search_hpp
#define FRAGILE_MIRRORS_best_first_search_hpp

#include <unordered_set>
#include <algorithm>

//#include "board_v2.hpp"


/// Score is a function or object with call operator, 
/// that receives one board argument and returns double that determines
/// how good the board is, the bigger the better 
template<class Board, class Score>
class BestFirstSearch {
    using CastNode = class Board::CastNode;
    using HashType = class Board::HashType;
    
    /**
     *  struct describes derivative from given board
     */
    struct Branch {
        Branch() {}
    
        Branch(shared_ptr<CastNode> node,
               HashType hash,
               double score) 
        : node(node), hash(hash), score(score) {}
    
        shared_ptr<CastNode> node;
        HashType hash;
        double score;
        
        /// going to sort it and collect best items
        /// so score should be as high as possible
        bool operator<(const Branch& b) const {
            return score > b.score;
        }
    };
        
        
    Score score_;
    ostream *output_;
    
    
    vector<Branch> Derivatives(Board& b, Count max_count) {
        Count sz = b.size();
        vector<Branch> derivs;
        for (int i = 0; i < sz; ++i) {
            array<Position, 4> cs = { { {i, -1}, {-1, i}, {i, sz}, {sz, i} } };
            for (auto& c : cs) {
                if (b.Cast(c) > 0) {
                    derivs.emplace_back(b.HistoryCasts(), b.hash(), score_(b));
                }
                b.Restore();
            }
        }
        int count = min<Count>(max_count, derivs.size());
        nth_element(derivs.begin(), derivs.begin()+count-1, derivs.end());
        derivs.resize(count);
        return derivs;
    }
    
    vector<Branch> Derivatives(Board& b, Count max_count, vector<Position>& isolated) {
        Count sz = b.size();
        vector<Branch> derivs;
        for (int i = 0; i < sz; ++i) {
            array<Position, 4> cs = { { {i, -1}, {-1, i}, {i, sz}, {sz, i} } };
            for (auto& c : cs) {
                if (b.Cast(c) > 0) {
                    if (b.IsLastIsolated()) {
                        isolated.push_back(c);
                    } else {
                        derivs.emplace_back(b.HistoryCasts(), b.hash(), score_(b));
                    }
                }
                b.Restore();
            }
        }
        int count = min<Count>(max_count, derivs.size());
        nth_element(derivs.begin(), derivs.begin()+count-1, derivs.end());
        derivs.resize(count);
        return derivs;
    }

    
    
public:
    Board Destroy(const Board& b) {
        unordered_set<HashType> explored;
        int per_level_count = 10;
        
        vector<vector<Branch>> next(2*b.size());
        // while time is not up
        bool just_started = true;
        int i = 0;
        while (true) {
            // output should be made though some sort of handler
            if (++i % 1000 == 0) cout << i << endl;
            Board next_b;
            int level;
            if (just_started) {
                just_started = false;
                next_b = b;
                level = -1;
            } else {
                Count next_sz = next.size();
                lol:
                level = uniform_int_distribution<>{0, next_sz-1}(RNG);
                auto& nt = next[level];
                if (nt.size() == 0) goto lol;
                next_b = b.InitWithCasts(nt.begin()->node);
                explored.insert(nt.begin()->hash);
                nt.erase(nt.begin());
            }
            while (++level != next.size() && !next_b.AllDestroyed()) {
                vector<Branch> ders = Derivatives(next_b, per_level_count);
                auto rem_it = remove_if(ders.begin(), ders.end(), [&](const Branch& br) {
                    return explored.count(br.hash) != 0;
                });
                ders.erase(rem_it, ders.end());
                auto& nt = next[level];
                // if merging changes next, then can continue with the best of recent else go out
                nt.insert(nt.end(), ders.begin(), ders.end());
                sort(nt.begin(), nt.end());
                nt.resize(min<Count>(nt.size(), per_level_count));
                auto it = find_if(nt.begin(), nt.end(), [&](const Branch& br) {
                    return br.node->previous == next_b.HistoryCasts(); 
                });
                if (it == nt.end()) {
                    break;
                }
                next_b.Cast(it->node->cast);
                explored.insert(next_b.hash());
                nt.erase(it);
           }     
           if (next_b.AllDestroyed()) {
               cout << next_b.CastCount() << endl;
               next.resize(next_b.CastCount());
           }       
        }
        
    }

    void set_score(Score score) {
        score_ = score;
    }
    
    void set_ostream(ostream& outout) {
        output_ = &output
    }
};



#endif
