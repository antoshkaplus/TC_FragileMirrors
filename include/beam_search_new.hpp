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
	
	// later can make method with another argument, like functor
	// at some point we have to shrink most certainly
    vector<Derivative> ExtractPromotion(int count) {
		sz = min(count, derivs_.size());
		nth_element(derivs_.begin(), derivs_.begin()+sz-1, derivs_.end(), std::greater<Derivative>());
		auto res(derivs_.begin(), derivs_.begin()+sz);
		derivs_.erase(derivs_.begin(), derivs_.begin()+sz);
		return res;
	}

    void Insert(const Derivative& d) {
		derivs_.push_back(d);
	}

	void PushAll(const vector<Derivative>& derivs) {
		derivs_.insert(derivs_.end(), derivs.begin(), derivs.end());
	}
	
private:
	vector<Derivative> derivs_;
	
};

// trying to incorporate dynamic width

// many things are probably better off implemented outside

class BeamSearchNew {
    // can make use of more parametered possibly


    // may not be a
    //using LevelDerivatives = vector<Derivative>;

    void Destroy(const Board& b) {

        level_derivs.resize(max_levels());
        PromoteBoardToLevel(b, 0);
        auto level_count = max_levels();

        // we need some kind of timer here
        // like start and end
        Timer t(10);
		int start_level = 0;
        while (!t.timeout()) {
			// should be while solution not found or takes not less than current solution
            // we start from prominent level
			for (int i = start_level; i < level_count; ++i) {
				PromoteLevel(i);
				if (t.timeout()) {
					break;
				}
            }
            start_level = ProminentLevel();
        }

        // lets say i'm in the middle of something
		
		// serve solution
    }

    int max_levels(); // computed from board size

	// we can just depend on current soltuion.
	// we can find current solution with some kind of stupid algorithm.
    int level_count(); // changes over time once best solution is found

    void PromoteLevel(int i) {
		// best of the best
        auto promo = level_derivs[i].ExtractPromotion(promotion_count_);
		UpdateScoreStatsWithLevelPromotion(i, promo);
        // or we could build really big vector and push everything. but to me it could be done like a bunch or lambdas
        for (auto d : promo) {
            auto b = *st.board;
            b.Cast(st.cast);
			if (b.AllDestroyed()) {
				SetSolution(b);
				break;
			}
            auto dd = ComputeBoardDerivatives(b);
            level_derivs[i+1].PushAll(dd);
        }
    }
	
	// should we put it in another data structure???
	void UpdateScoreStatsWithLevelPromotion(int level, const vector<Derivative>& derivs) {
		auto& stats = level_score_stats[level];
		stats.min = min(stats.min, *MinElement(derivs.begin(), derivs.end(), bind(&Derivative::score)));
		stats.max = max(stats.max, *MaxElement(derivs.begin(), derivs.end(), bind(&Derivative::score)));
	}
	
	void UpdateScoreStatsWithSolution(const Board& board) {
		Board b = original_;
		auto& casts = board.CastHistory();
		// last one is not relevant as we look for even better solutions
		for (auto i = 0; i < board.CastCount()-1; ++i) {
			b.Cast(casts[i]);
			stats[i].best = score;
		}
	}
	
	void SetSolution(Board b) {
		// set new bound on number of casts
		level_derivs.resize(b.CastCount()-1);
		// now we have to update our level_score_stats
		level_score_stats.resize(b.CastCount()-1);
		UpdateScoreStatsWithSolution(sol);
		solution_ = sol;
	}
	
    void PromoteBoardToLevel(Board& b, int level) {
		derivs = ComputeBoardDerivatives(b);
		level_derivs[level].PushAll(derivs);
	}

    vector<Derivative> ComputeBoardDerivatives(Board& b) {
        vector<Derivative> res;
        auto b_ptr = make_shared<Board>(b);
        for (auto p : b.CastCandidates()) {
            b.Cast(p);
			// if cast is empty, board has to be discovered
            if (discovered_.count(b.hash()) == 0) {
                Derivative st(b_ptr, p, score_(b), b.hash());
                res.push_back(st);
				discovered_.insert(b.hash());
            }
            // somewhere we have to check if it;s finish and if yes we should reduce number of levels
            b.Restore();
        }
        return res;
    }
	
	int ProminentLevel() {
		int i = MaxElement(level_score_stats.begin(), level_score_stats.end(), [](ScoreStats& ss) {
			return (ss.best - ss.min) / (ss.max - ss.min); 
		}) - level_score_stats.begin();
		return i;
	}
	
	struct ScoreStats {
		double min;
		double max;
		double best;
	};
	
	
	// after we do the cast we come to the level
    vector<LevelDerivatives> level_derivs;
    vector<ScoreStats> level_score_stats;
	int promotion_count_;
	// we have to test to use per level
	// global sounds better because of end cases
	// I think we should keep how many casts were taken,
	// sometimes it can be that using some badass technique we arrived to the same location with fewer casts
	// but sometimes it can be reverse of course
	unordered_set<HashType> discovered_;
	Board solution_;
	Board original_;
	Score score_;
};
