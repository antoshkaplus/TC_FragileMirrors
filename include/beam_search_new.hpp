//
// Created by Anton Logunov on 4/1/17.
//
#pragma once

#include "util.hpp"
#include "board_v1.hpp"
#include "score.hpp"


using BoardType = Board_v1;
using HashType = Board_v1::HashType;
using Score = Score_v1<Board_v1>;
using Board = Score_v1;


struct Derivative {
    shared_ptr<const BoardType> board;
    Position cast;
    double score;
    HashType hash;

    Derivative() {}
    Derivative(shared_ptr<const BoardType> board, Position cast, double score, HashType hash)
            : board(board), cast(cast), score(score), hash(hash) {}

    bool operator<(const Derivative& s) const {
        return score < s.score;
    }
};


// it's a little bit different
struct LevelDerivatives {

    vector<Derivative> ExtractPromotion();

    double Potential(BoardType& cur_sol_level_board);

    void Insert(Derivative);

};

// trying to incorporate dynamic width

// many things are probably better off implemented outside

class BeamSearchNew {
    /// can make use of more parametered possibly


    // may not be a
    //using LevelDerivatives = vector<Derivative>;




    void Destroy(const Board& b) {

        level_derivs.resize(max_levels());
        PromoteBoardToLevel(b, 0);
        auto level_count = max_levels();

        // we need some kind of timer here
        // like start and end
        Timer t(10);
        while (!t.timeout()) {
            for (int i = 0; i < level_count; ++i) {
                PromoteLevel(i);
            }
            WeakenPrunningRule();
        }

        // lets say i'm in the middle of something

    }

    int max_levels(); // computed from board size

    int level_count(); // changes over time once best solution is found

    void PromoteLevel(int i) {
        auto promo = level_derivs[i].ExtractPromotion();
        // or we could build really big vector and push everything. but to me it could be done like a bunch or lambdas
        for (auto d : promo) {
            auto b = *st.board;
            b.Cast(st.cast);
            auto dd = ComputeBoardDerivatives(b);
            level_derivs[i+1].PushAll(dd);
        }
    }

    void PromoteBoardToLevel(Board& b, int level);

    vector<Derivative> ComputeBoardDerivatives(Board& b) {
        vector<Derivative> res;
        auto b_ptr = make_shared<Board>(b);
        for (auto p : b.CastCandidates()) {
            b.Cast(p);
            if (discovered_.count(b.hash()) == 0) {
                Derivative st(b_ptr, p, score_(b), b.hash());
                res.push_back(st);
            }
            // somewhere we have to check if it;s finish and if yes we should reduce number of levels
            b.Restore();
        }
        return res;
    }

    void WeakenPruningRule() {
        for (LevelDerivatives& ders : level_derivs) {
            // weaken prunning rule for each item
        }
    }

    vector<LevelDerivatives> level_derivs;
    vector<double> level_best_score;
};