//
//  greedy.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/28/14.
//
//

#ifndef FRAGILE_MIRRORS_greedy_hpp
#define FRAGILE_MIRRORS_greedy_hpp

#include "board_v3.hpp"

struct Greedy {
    using Board = Board_v3;

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
    
    Board Destroy(const Board& b, Count width) {
        // need to put inside cast and order number
        stack<Previous> previous;
        auto comp = [](const Derivative& d_0, const Derivative& d_1) {
            return d_0.destroyed_count > d_1.destroyed_count;
        };
        Board board = b;
        Count best_destroyed = 10000;
        while (true) {
            while (!board.AllDestroyed()) {
                CastIterator it(b.size());
                vector<Derivative> ders;
                while (it.HasNext()) {
                    Position c = it.Next();
                    board.Cast(c);
                    ders.emplace_back(c, board.MirrorsDestroyed());
                    board.Restore();
                }    
                Count count = min<Count>(ders.size(), width); 
                nth_element(ders.begin(), ders.begin() + count -1, ders.end(), comp);
                if (ders.front().destroyed_count == b.size()*b.size()) {
                    board.Cast(ders.front().cast);
                    break;
                }
                
                Derivative best_second;
                best_second.destroyed_count = 0;
                for (auto& p : ders) {
                    board.Cast(p.cast);
                    CastIterator it(b.size());
                    while (it.HasNext()) {
                        Position c = it.Next();
                        board.Cast(c);
                        if (board.MirrorsDestroyed() > best_second.destroyed_count) {
                            best_second.destroyed_count = board.MirrorsDestroyed();
                            best_second.cast = p.cast;
                        }
                        board.Restore();
                    }
                    board.Restore();
                 }
                 if (ders.front().cast != best_second.cast) {
                     previous.emplace(ders.front().cast, board.CastCount());
                 }
                 board.Cast(best_second.cast);
            }
            if (best_destroyed > board.CastCount()) {
                cout << "start: " << board.CastCount() << " prev: " << previous.size() << endl; 
                best_destroyed = board.CastCount();
            }
            if (previous.empty()) break;
            Previous pr = previous.top();
            previous.pop();
            while (board.CastCount() != pr.cast_count) {
                board.Restore();
            }
            board.Cast(pr.cast); 
         }
         return board;
    }
    


};


#endif
