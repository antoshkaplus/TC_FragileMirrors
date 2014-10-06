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

<<<<<<< HEAD
#include "ant/core.h"
#include <ant/optimization.h>
=======
#include <ant>

>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250

#include "board.h"


struct Solver {
    Solver() : output("out.txt") {}
    virtual vector<int> destroy(vector<string>& board) = 0;
    ofstream output;
};

struct NaiveSolver : Solver {
    vector<int> destroy(vector<string>& board) override {
        vector<int> int_moves;
        Board sb(board);
        while (true) {
            bool again = false;
            for (auto i = 0; i < board.size(); ++i) {
<<<<<<< HEAD
                if (sb.rowMirrorsLeft(i) > 0) {
=======
                if (sb.mirrors_left_[sb.kHor][i] > 0) {
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
                    sb.cast({i, -1});
                    int_moves.push_back(i);
                    int_moves.push_back(-1);
                    again = true;
                }
            }
            if (!again) break;
        }
        return int_moves;
    }
};

struct BeamSearchSolver : Solver, opt::BeamSearch {
    
    using BS = opt::BeamSearch;
    
    // simple double 
    struct Value : BS::Value {
        Value(double val) : val(val) {}
<<<<<<< HEAD
        
=======
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
        bool operator<(const BS::Value& v) const override {
            return val < static_cast<const Value&>(v).val;
        }
        // need to consider count of moves
        double val;
    };
    
    // cast position
    struct Component : BS::Component, Position {
        Component(Int row, Int col) : Position(row, col) {}
    };
    
    // everyone is candidate 
    struct State : BS::State {
        
        State(const Board& board) : board_(board) {}
        
        bool isFinal() const override {
            return board_.mirrorsLeft() == 0;
        }

        virtual vector<unique_ptr<BS::Component>> candidateComponents() {
            vector<unique_ptr<BS::Component>> ccs;            
            for (auto i = 0; i < board_.size(); ++i) {
<<<<<<< HEAD
                if (board_.rowMirrorsLeft(i) > 0) {
                    ccs.emplace_back(new Component(i, -1));
                    ccs.emplace_back(new Component(i, (Int)board_.size()));
                }
                if (board_.colMirrorsLeft(i) > 0) {
                    ccs.emplace_back(new Component(-1, i));
                    ccs.emplace_back(new Component((Int)board_.size(), i));
=======
                if (board_.mirrors_left_[board_.kHor][i] > 0) {
                    ccs.emplace_back(new Component(i, -1));
                    ccs.emplace_back(new Component(i, board_.size()));
                }
                if (board_.mirrors_left_[board_.kVer][i] > 0) {
                    ccs.emplace_back(new Component(-1, i));
                    ccs.emplace_back(new Component(board_.size(), i));
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
                }
            }
            return ccs;
        }

        void push(const opt::BeamSearch::Component& comp) override {   
            const Component& c = static_cast<const Component&>(comp);
            int_casts_.push_back(c.row);
            int_casts_.push_back(c.col);
            board_.cast(c);
        }
        
        // maybe board should keep all the canst.... and board needs manager to keep track of restore... possiblity
        void pop() override {
            int_casts_.pop_back();
            int_casts_.pop_back();
            board_.restore();
        } 
        
        // can remake it later
        virtual unique_ptr<BS::Value> value() const override {
            Count m_d = board_.mirrorsDestroyed();
            return unique_ptr<BS::Value>(new Value((double)int_casts_.size()/m_d));
        }
        
        virtual State& operator=(const BS::State& s) override {
            return *this = dynamic_cast<const State&>(s);
        }
        
        virtual unique_ptr<BS::State> clone() const override {
            return unique_ptr<BS::State>(new State(*this));
        }
        
        virtual bool operator==(const BS::State& state) const override {
            return board_ == static_cast<const State&>(state).board_;
        }
        
        
        Board board_;
        // kCastOne kCastDouble
        vector<char> cast_type_;
        vector<int> int_casts_; 
    };
    
    struct NarrowState : State {
        
        NarrowState(const Board& board) : State(board) {}
        
        vector<double> values;
        vector<Index> order;
        
        virtual vector<unique_ptr<BS::Component>> candidateComponents() {
            vector<unique_ptr<BS::Component>> cands = State::candidateComponents();
            values.resize(cands.size());
            for (auto i = 0; i < cands.size(); ++i) {
                values[i] = board_.tryCast(static_cast<Component&>(*cands[i]));
            }
            order.resize(cands.size());
            iota(order.begin(), order.end(), 0);
            Count count = min<Count>(10, cands.size());
            partial_sort(order.begin(), order.begin()+count, order.end(), 
                         [&](Index i_0, Index i_1) { return values[i_0] > values[i_1]; });
            decltype(cands) res(count);
            for (auto i = 0; i < count; ++i) res[i] = std::move(cands[order[i]]);
            return res;
        }
        
        virtual NarrowState& operator=(const BS::State& s) override {
            return *this = dynamic_cast<const NarrowState&>(s);
        }
        
        virtual unique_ptr<BS::State> clone() const override {
            return unique_ptr<BS::State>(new NarrowState(*this));
        }
    };
    
    
    
    using CurrentState = NarrowState;
    vector<int> destroy(vector<string>& board) override {
        Board sb(board);
        BeamSearchSolver bss;
<<<<<<< HEAD
        Count count = board.size() < 75 ? 1000 : 500;  
=======
        Count count = board.size() < 75 ? 150 : 85;  
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
        //Count count = 1000;
        auto ptr = bss.solve(CurrentState(sb), count);
        return static_cast<CurrentState&>(*ptr).int_casts_;
    }
};












#endif /* defined(__FragileMirrors__solver__) */
