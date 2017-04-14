//
// Created by Anton Logunov on 4/1/17.
//
#pragma once

#include <functional>

#include "board_v1_impl_1.hpp"
#include "naive_search.hpp"
#include "derivative.hpp"
#include "discovery.hpp"

using namespace std::placeholders;


template <class Board>
class BeamSearchNew {

    using BoardType = Board;
    using HashType = typename Board::HashType;
    using ScoreType = Score_v1;
    using CastType = typename Board::CastType;

    using Derivative = Derivative<Board>;
    using LevelDerivatives = LevelDerivatives<Derivative>;

public:
    Board Destroy(const Board& b) {
		original_ = b;
		InitializeSolution();

        PromoteBoardToLevel(b, 0);

        // we need some kind of timer here
        // like start and end
        Timer t(millis_);
		int start_level = 0;
        while (!t.timeout()) {
			// should be while solution not found or takes not less than current solution
            // we start from prominent level
			for (int i = start_level; i < level_count()-1; ++i) {
				if (!PromoteLevel(i)) break;
				if (t.timeout()) {
					break;
				}
            }
            start_level = ProminentLevel();
        }

        // lets say i'm in the middle of something
		
		// serve solution
		return solution_;
    }

	void set_score(ScoreType& s) {
		score_ = s;
	}

	void set_beam_width(Count beam_width) {
		promotion_count_ = beam_width;
	}

    auto& level_derivs_used() const {
        return level_derivs_used_;
    }

    Count level_derivs_left(Index level) const {
        return level_derivs_[level].derivatives_left();
    }

    auto& level_derivs() const {
        return level_derivs_;
    }

    void set_millis(Count millis) {
        millis_ = millis;
    }

private:
    void InitializeSolution() {
        NaiveSearch<Board, Score> ns;
        solution_ = ns.Destroy(original_, score_);
        SetSolution(solution_);
        for (auto& stats : level_score_stats_) {
            stats.max = numeric_limits<double>::min();
            stats.min = numeric_limits<double>::max();
        }
    }


	// we can just depend on current soltuion.
	// we can find current solution with some kind of stupid algorithm.
    int level_count() {
		return level_derivs_.size();
	}

    // returns false on some kind of failure
    bool PromoteLevel(int i) {
		// best of the best
        auto promo = level_derivs_[i].ExtractPromotion(promotion_count_);
        level_derivs_used_[i] += promo.size();
        if (promo.empty()) {
            return false;
        }
		UpdateScoreStatsWithLevelPromotion(i, promo);
        // or we could build really big vector and push everything. but to me it could be done like a bunch or lambdas
        bool best_updated = false;
        for (auto st : promo) {
            auto b = static_cast<const Board&>(*st.board);
            b.Cast(st.cast);
			if (b.AllDestroyed()) {
				SetSolution(b);
				break;
			}
            auto dd = ComputeBoardDerivatives(b);
            best_updated |= level_derivs_[i+1].PushAll(dd);
        }
        level_derivs_[i+1].Cap();
        return best_updated;
    }

	// should we put it in another data structure???
	void UpdateScoreStatsWithLevelPromotion(int level, const vector<Derivative>& derivs) {
		auto& stats = level_score_stats_[level];
		auto score_functor = [](const Derivative& d) {return d.score;};
		stats.min = min(stats.min, MinElement(derivs.begin(), derivs.end(), score_functor)->score);
		stats.max = max(stats.max, MaxElement(derivs.begin(), derivs.end(), score_functor)->score);
	}
	
	void UpdateScoreStatsWithSolution(const Board& board) {
		Board b = original_;
		auto casts = board.CastRayHistory();
		// last one is not relevant as we look for even better solutions
		for (auto i = 0; i < board.CastCount()-1; ++i) {
            b.Cast(casts[i]);
			level_score_stats_[i].best = score_(b);
		}
	}
	
	void SetSolution(const Board& sol) {
		// set new bound on number of casts
		level_derivs_.resize(sol.CastCount()-1);
		// now we have to update our level_score_stats_
		level_score_stats_.resize(sol.CastCount()-1);
        level_derivs_used_.resize(sol.CastCount()-1);
		UpdateScoreStatsWithSolution(sol);
		solution_ = sol;
	}
	
    void PromoteBoardToLevel(const Board& b, int level) {
		auto derivs = ComputeBoardDerivatives(b);
		level_derivs_[level].PushAll(derivs);
	}

    vector<Derivative> ComputeBoardDerivatives(Board b) {
        vector<Derivative> res;
        auto b_ptr = make_shared<Board>(b);
        b.ForEachAppliedCast([&](CastType cast){
            // if cast is empty, board has to be discovered
            if (discovery_.Discover(b)) {
                Derivative st(b_ptr, cast, score_(b), b.hash());
                res.push_back(st);
            }
            // somewhere we have to check if it's finish and if yes we should reduce number of levels
        });
        return res;
    }
	
	int ProminentLevel() {
		auto func = [](ScoreStats& ss) {
			return (ss.best - ss.min) / (ss.max - ss.min);
		};
		int i = MaxElement(level_score_stats_.begin(), level_score_stats_.end(), func) - level_score_stats_.begin();
        return i;
	}
	
	struct ScoreStats {
		double min;
		double max;
		double best;
	};

	// after we do the cast we come to the level
    vector<LevelDerivatives> level_derivs_;
    vector<ScoreStats> level_score_stats_;
    vector<Count> level_derivs_used_;
	int promotion_count_;
    DiscoveryNew discovery_;
	Board solution_;
	Board original_;
	ScoreType score_;
    Count millis_{30000};
};
