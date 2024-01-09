//
//  solver.h
//  FragileMirrors
//
//  Created by Anton Logunov on 5/7/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __FragileMirrors__solver__
#define __FragileMirrors__solver__

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <forward_list>

#include "ant/core/core.hpp"

#include "board.hpp"


using namespace std;


struct Solver {
    Solver() : output("out.txt") {}
    virtual vector<int> destroy(vector<string>& board) = 0;
    ofstream output;
};

template <class Board>
struct NaiveSolver : Solver {
    vector<int> destroy(vector<string>& board) override {
        Count sz = board.size();
        Board sb(board);
        while (true) {
            bool again = false;
            for (int i = 0; i < board.size(); ++i) {
                if (sb.Cast({i, -1}) > 0) again = true;
                sb.Cast({i, sz});
                if (sb.Cast({-1, i}) > 0) again = true;
                sb.Cast({sz, i});
            }
            if (!again) break;
        }
        vector<int> int_moves;
        for (auto& p : sb.HistoryPositions()) {
            int_moves.push_back(p.row);
            int_moves.push_back(p.col);
        }
        return int_moves;
    }
};

//struct BeamSearchSolver : Solver, opt::BeamSearch {
//    
//    using BS = opt::BeamSearch;
//    
//    // simple double 
//    struct Value : BS::Value {
//        Value(double val) : val(val) {}
//        
//        bool operator<(const BS::Value& v) const override {
//            return val < static_cast<const Value&>(v).val;
//        }
//        // need to consider count of moves
//        double val;
//    };
//    
//    // cast position
//    struct Component : BS::Component, Position {
//        Component(Int row, Int col) : Position(row, col) {}
//    };
//    
//    // everyone is candidate 
//    struct State : BS::State {
//        
//        State(const Board& board) : board_(board) {}
//        
//        bool isFinal() const override {
//            return board_.mirrorsLeft() == 0;
//        }
//
//        virtual vector<unique_ptr<BS::Component>> candidateComponents() {
//            vector<unique_ptr<BS::Component>> ccs;            
//            for (auto i = 0; i < board_.size(); ++i) {
//                if (board_.rowMirrorsLeft(i) > 0) {
//                    ccs.emplace_back(new Component(i, -1));
//                    ccs.emplace_back(new Component(i, (Int)board_.size()));
//                }
//                if (board_.colMirrorsLeft(i) > 0) {
//                    ccs.emplace_back(new Component(-1, i));
//                    ccs.emplace_back(new Component((Int)board_.size(), i));
//                }
//            }
//            return ccs;
//        }
//
//        void push(const opt::BeamSearch::Component& comp) override {   
//            const Component& c = static_cast<const Component&>(comp);
//            int_casts_.push_back(c.row);
//            int_casts_.push_back(c.col);
//            board_.cast(c);
//        }
//        
//        // maybe board should keep all the canst.... and board needs manager to keep track of restore... possiblity
//        void pop() override {
//            int_casts_.pop_back();
//            int_casts_.pop_back();
//            board_.restore();
//        } 
//        
//        // can remake it later
//        virtual unique_ptr<BS::Value> value() const override {
//            Count m_d = board_.mirrorsDestroyed();
//            return unique_ptr<BS::Value>(new Value((double)int_casts_.size()/m_d));
//        }
//        
//        virtual State& operator=(const BS::State& s) override {
//            return *this = dynamic_cast<const State&>(s);
//        }
//        
//        virtual unique_ptr<BS::State> clone() const override {
//            return unique_ptr<BS::State>(new State(*this));
//        }
//        
//        virtual bool operator==(const BS::State& state) const override {
//            return board_ == static_cast<const State&>(state).board_;
//        }
//        
//        
//        Board board_;
//        // kCastOne kCastDouble
//        vector<char> cast_type_;
//        vector<int> int_casts_; 
//    };
//    
//    struct NarrowState : State {
//        
//        NarrowState(const Board& board) : State(board) {}
//        
//        vector<double> values;
//        vector<Index> order;
//        
//        virtual vector<unique_ptr<BS::Component>> candidateComponents() {
//            vector<unique_ptr<BS::Component>> cands = State::candidateComponents();
//            values.resize(cands.size());
//            for (auto i = 0; i < cands.size(); ++i) {
//                values[i] = board_.tryCast(static_cast<Component&>(*cands[i]));
//            }
//            order.resize(cands.size());
//            iota(order.begin(), order.end(), 0);
//            Count count = min<Count>(10, cands.size());
//            partial_sort(order.begin(), order.begin()+count, order.end(), 
//                         [&](Index i_0, Index i_1) { return values[i_0] > values[i_1]; });
//            decltype(cands) res(count);
//            for (auto i = 0; i < count; ++i) res[i] = std::move(cands[order[i]]);
//            return res;
//        }
//        
//        virtual NarrowState& operator=(const BS::State& s) override {
//            return *this = dynamic_cast<const NarrowState&>(s);
//        }
//        
//        virtual unique_ptr<BS::State> clone() const override {
//            return unique_ptr<BS::State>(new NarrowState(*this));
//        }
//    };
//    
//    
//    
//    using CurrentState = NarrowState;
//    vector<int> destroy(vector<string>& board) override {
//        Board sb(board);
//        BeamSearchSolver bss;
//        Count count = board.size() < 75 ? 1000 : 500;
//        count = 2000;  
//        //Count count = 1000;
//        auto ptr = bss.solve(CurrentState(sb), count);
//        return static_cast<CurrentState&>(*ptr).int_casts_;
//    }
//    
//    Board Destroy(const Board& board, Count beam_width) {
//        BeamSearchSolver bss;
//        auto ptr = bss.solve(NarrowState(board), beam_width);
//        return static_cast<NarrowState&>(*ptr).board_;
//    }
//};

/*
struct BB {
    struct Tile {
        // some getter methods
    private:
        char mirror;
        short tile_indices_[4];
    };
    
    BB(const vector<string>& board) {
        short sz = board.size()
        short elem_count = board.size()*board.size();
        tiles_.resize(sz*sz + 4*sz);
        
    }
    
    
     TryCast()
    
     vector<shared_ptr<Board>> GetAfterBestCasts

private:
    // where we go out is -1 by default
    vector<Tile> tiles_;
    // outer tiles should show where to go next, should be able to keep direction
    vector<Tile> outer_tiles_;
};
*/


struct MultiDerivative {
    MultiDerivative(shared_ptr<Board> board, Position cast)
    : board(board), casts({cast}) {}

    shared_ptr<Board> board;
    // casts on board to get to derivative 
    forward_list<Position> casts;
    Count destroy_count;
};

MultiDerivative Derivate(shared_ptr<Board> board, Position cast);


template<class Comp>
using pr_queue = priority_queue<Board, vector<Board>, Comp>;


// need factory of boards

//
//struct BestFirstBeamSearch {
//
//private:
//    static double Score(const Board& board) {
//        double score = 2 * board.mirrorsDestroyed() + 1;
//        for (auto i = 0; i < board.size(); ++i) {
//            score += (board.rowMirrorsLeft(i) == 0) + (board.colMirrorsLeft(i) == 0);
//            score -= (board.rowMirrorsLeft(i) == 1) + (board.colMirrorsLeft(i) == 1);
//            score -= (board.rowMirrorsLeft(i) == 3) + (board.colMirrorsLeft(i) == 3);
//            score -= (board.rowMirrorsLeft(i) == 5) + (board.colMirrorsLeft(i) == 5);
//        }
//        return 1/score;
//    }
//    
//    // going to solve minimization problem
//    struct Item {
//        shared_ptr<const Board> board;
//        Position cast;
//        double score;
//        
//        Item(shared_ptr<const Board> board,
//             Position cast,
//             double score) : board(board), cast(cast), score(score) {}
//        
//        // need minimum queue, default is maximum
//        bool operator<(const Item& item) const {
//            return score < item.score;
//        }
//    };
//
//    // i won't change but after return you maybe want to change
//    // probably should return index?
////    static Index BestItem(const vector<Item>& items) {
////        return min_element(items.begin(), items.end(), [](const Item& i_0, const Item& i_1) {
////            return i_0.score < i_1.score;
////        }) - items.begin();
////    }
////
////    static shared_ptr<Board> BoardAfterCast(const Board& board, const Position& cast) {
////        res->cast(cast);
////    }
//    
//    static shared_ptr<Board> BoardCopy(const Board& board) {
//        Board* res = new Board();
//        *res = board;
//        return shared_ptr<Board>{res};
//    }
//
//public:
//
//    // can wait with copying shit. while not ready to process
//    Board Destroy(const Board& board, Count beam_width) {
//        Board res;
//        Count res_cast_count = numeric_limits<Count>::max();
//        
//        Count sz = board.size();
//        set<Item> solution;
//        
//        solution.emplace(BoardCopy(board), Position{0, 0}, Score(board));
//        while (!solution.empty()) {
//            const Item& item = *solution.begin();
//            shared_ptr<Board> board = BoardCopy(*item.board);
//            if (item.cast != Position{0, 0}) board->cast(item.cast);
//            if (board->mirrorsLeft() == 0 && board->CastCount() < res_cast_count) {
//                 res_cast_count = board->CastCount();
//                 res = *board;
//            }
//            for (auto i = 0; i < sz; ++i) {
//                Position pp[4] = {{-1, i}, {i, -1}, {sz, i}, {i, sz}};
//                for (auto k = 0; k < 4; ++k) {
//                    board->cast(pp[k]);
//                    if (hashes.count(board->Hash()) != 0) {
//                        continue;
//                    }
//                    double sc = Score(*board);
//                    if (solution.size() < beam_width) {
//                        solution.emplace(board, pp[k], sc);
//                        hashes.insert(board->Hash());
//                    } else if (solution.rbegin()->score > sc) {
//                        auto it = solution.end();
//                        solution.erase(--it);
//                        solution.emplace(board, pp[k], sc);
//                        hashes.insert(board->Hash());
//                    }
//                    board->restore();
//                }
//            }
//            
//        }
//        return res;
//    }
//    
//    unordered_set<Board::HashType> hashes;
//};








#endif /* defined(__FragileMirrors__solver__) */
