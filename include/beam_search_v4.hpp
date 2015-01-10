//
//  beam_search_v4.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 1/5/15.
//
//

#ifndef FRAGILE_MIRRORS_beam_search_v4_hpp
#define FRAGILE_MIRRORS_beam_search_v4_hpp

#include "board_v4.hpp"

template<class Board>
struct BeamSearch_v2 {
private:
    using Hash = typename Board::HashType; 
    
    struct Derivative {
        Derivative() {}
        Derivative(Board* b, short r, Hash h, double s) 
        : origin(b), hash(h), score(s), ray_index(r) {}
        
        Board* origin;
        Hash hash;
        double score;
        short ray_index;
    };
    
    double empty_lines_param_{0};
    double overhead_param_{1};
    
    // our goal is to minimize
    double Score(const Board& b) const {
        return - (b.MirrorsDestroyed() + empty_lines_param_ * b.EmptyLinesCount());
    }
    
public:
    using BoardType = Board;
    using Hashes = unordered_set<typename Board::HashType>;
    
    Board Destroy(const Board& board, Count beam_width, double empty_lines_param, double overhead_param) {
        empty_lines_param_ = empty_lines_param;
        overhead_param_  = overhead_param;
        assert(overhead_param_ >= 1.);
        typename vector<Board>::iterator res;
        // has move semantics now 
        vector<Board> current;
        vector<Board> next;
        vector<Derivative> next_casts;  
        vector<Index> inds;
        Hashes hashes;
        current.push_back(board);
        while (true) {
            for (auto& c : current) {
                AddDerivatives(c, next_casts);
            }
            // current iteration changed
            // dublicates overhead
            SelectBoardIndexes(next_casts, inds, overhead_param_*beam_width);
            RemoveDublicates(next_casts, inds, hashes);
            inds.resize(min<Count>(inds.size(), beam_width));
            FillBoards(next_casts, inds, next);
            next_casts.clear();
            // should use move
            swap(next, current);
            if (current.begin()->AllDestroyed()) {
                break;
            } 
        }
        return *(current.begin());
    }    
    
private:

    // bs - where to write results
    void FillBoards(const vector<Derivative>& derivs, 
                    vector<Index>& inds,
                    vector<Board>& bs) {

        sort(inds.begin(), inds.end(), [&](Index i_0, Index i_1) {
            return derivs[i_0].origin < derivs[i_1].origin;
        });
        
        bs.resize(inds.size());
        vector<short> rays;
        Index i = 0;
        while (i < bs.size()) {
            Index start = i;
            rays.clear();
            rays.push_back(derivs[inds[start]].ray_index);
            while (++i < bs.size() && derivs[inds[start]].origin == derivs[inds[i]].origin) {
                rays.push_back(derivs[inds[i]].ray_index);
            }
            auto& b_or = *(derivs[inds[start]].origin); 
            for (Index k = 0; k < rays.size(); ++k) {
                bs[start + k] = b_or;
                bs[start + k].Cast(rays[k]);
            }
        }
    }
    
    void AddDerivatives(Board& b, vector<Derivative>& derivs) { 
        for (int i = 0; i < b.RayCount(); ++i) {
            if (b.CastRestorable(i) > 0) derivs.emplace_back(&b, i, b.hash(), Score(b));
            b.Restore();
        }
    }
    
    // here we have an oportunity to choose max_count good derivatives
    void SelectBoardIndexes(const vector<Derivative>& bs, vector<Index>& inds, Count max_count) {
        inds.resize(bs.size());
        iota(inds.begin(), inds.end(), 0);
        int count = min<int>(max_count, bs.size());
        nth_element(inds.begin(), inds.begin() + count-1, inds.end(), [&](Index i_0, Index i_1) {
            return bs[i_0].score < bs[i_1].score;
        });
        inds.resize(count);
    }   
    
    // changing inds a little
    void RemoveDublicates(const vector<Derivative>& next_casts, vector<Index>& inds, Hashes& hashes) {
        hashes.clear();
        for (int i = 0; i < inds.size();)  {
            Hash h = next_casts[inds[i]].hash;
            if (hashes.count(h) != 0) {
                swap(inds[i], inds.back());
                inds.pop_back();
                continue;
            }
            hashes.insert(h);
            ++i;
        }
    }
};



#endif
