//
// Created by Anton Logunov on 4/1/17.
//
#pragma once

#include <functional>

#include "util.hpp"
#include "board_v1_impl_1.hpp"
#include "cast_history.hpp"
#include "score.hpp"
#include "naive_search.hpp"

using namespace std::placeholders;



struct Derivative {
    shared_ptr<const Board_v1> board;
    Position cast;
    double score;
    Board_v1::HashType hash;

    Derivative() {}
    Derivative(shared_ptr<const Board_v1> board, Position cast, double score, Board_v1::HashType hash)
            : board(board), cast(cast), score(score), hash(hash) {}

    bool operator<(const Derivative& s) const {
        return score < s.score;
    }

	bool operator>(const Derivative& s) const {
		return score > s.score;
	}
};


// it's a little bit different
struct LevelDerivatives {
	
	// later can make method with another argument, like functor
	// at some point we have to shrink most certainly
    vector<Derivative> ExtractPromotion(int count) {
		auto sz = min<Count>(count, derivs_.size());
		nth_element(derivs_.begin(), derivs_.begin()+sz-1, derivs_.end(), std::greater<Derivative>());
		vector<Derivative> res(derivs_.begin(), derivs_.begin()+sz);
		derivs_.erase(derivs_.begin(), derivs_.begin()+sz);
		return res;
	}

    void Insert(const Derivative& d) {
		derivs_.push_back(d);
	}

	bool PushAll(const vector<Derivative>& derivs) {
		derivs_.insert(derivs_.end(), derivs.begin(), derivs.end());
	    return true;
    }

    void Cap() {
        if (derivs_.size() > cap_) {
            nth_element(derivs_.begin(), derivs_.begin()+cap_-1, derivs_.end(), std::greater<Derivative>());
            derivs_.erase(derivs_.begin()+cap_, derivs_.end());
        }
    }

    Count derivatives_left() const {
        return derivs_.size();
    }

private:
	vector<Derivative> derivs_;
    int cap_{20000};
};


// elements with highest score go first
struct LevelDerivativesNew {

    // later can make method with another argument, like functor
    // at some point we have to shrink most certainly
    vector<Derivative> ExtractPromotion(int count) {
        auto res = best_;
        best_.clear();

        auto sz = min<Count>(promotion_count_, rest_.size());
        nth_element(rest_.begin(), rest_.begin()+sz-1, rest_.end(), std::greater<Derivative>());
        best_.insert(best_.begin(), rest_.begin(), rest_.begin()+sz);
        sort(best_.begin(), best_.end(), std::greater<Derivative>());
        rest_.erase(rest_.begin(), rest_.begin()+sz);

        return res;
    }

    // we consider that if best is empty rest is also should be empty
    bool PushAll(vector<Derivative>& derivs) {
        bool best_updated = false;
        if (best_.size() < promotion_count_) {
            auto sz = min(promotion_count_-best_.size(), derivs.size());
            if (sz > 0) best_updated = true;
            best_.insert(best_.end(), derivs.begin()+derivs.size()-sz, derivs.end());
            derivs.erase(derivs.begin()+derivs.size()-sz, derivs.end());
            sort(best_.begin(), best_.end(), std::greater<Derivative>());
        }
        for (auto& d : derivs) {
            if (best_.back().score < d.score) {
                best_updated = true;
                auto b = best_.back();
                best_.pop_back();
                best_.insert(upper_bound(best_.begin(), best_.end(), d, std::greater<Derivative>()), d);
                d = b;
            }
        }
        rest_.insert(rest_.end(), derivs.begin(), derivs.end());

        return best_updated;
    }

    void Cap() {
        if (rest_.size() > cap_) {
            nth_element(rest_.begin(), rest_.begin()+cap_-1, rest_.end(), std::greater<Derivative>());
            rest_.erase(rest_.begin()+cap_, rest_.end());
        }
    }

    Count derivatives_left() const {
        return best_.size() + rest_.size();
    }

private:

    void ReplenishBest() {

    }

    int promotion_count_{100};
    int cap_{20000};
    vector<Derivative> best_;
    vector<Derivative> rest_;
};


struct Discovery {
    using Board = Board_v1;

    bool Discover(const Board& b) {
        return discovered_.insert(b.hash()).second;
    }

private:

    unordered_set<Board::HashType> discovered_;

};

struct DiscoveryNew {
    using Board = Board_v1;

    bool Discover(const Board& b) {
        auto it = discovered_.find(b.hash());
        if (it == discovered_.end() || it->second > b.CastCount()) {
            discovered_[b.hash()] = b.CastCount();
            return true;
        }
        return false;
    }

private:
    // look for less count of cast on discover
    unordered_map<Board::HashType, Count> discovered_;

};


// result is little bit worse because we lose board state info with lower amount of bits
struct DiscoveryFast {
    static constexpr int BIT_COUNT = 28;

    using Board = Board_v1;

    DiscoveryFast() {
        discovered_.resize(2 << BIT_COUNT, 0);
    }

    bool Discover(const Board& b) {
        auto index = ((uint32_t)b.hash().to_ulong() << (32-BIT_COUNT)) >> (32-BIT_COUNT);
        if (discovered_[index] == 0 || discovered_[index] > b.CastCount()) {
            discovered_[index] = b.CastCount();
            return true;
        }
        return false;
    }

private:
    vector<uint8_t> discovered_;
};


// trying to incorporate dynamic width

// many things are probably better off implemented outside

class BeamSearchNew {

    using BoardType = Board_v1_Impl_1<CastHistory_Nodes>;
    using HashType = Board_v1::HashType;
    using Score = Score_v1;
    using Board = BoardType;

    // can make use of more parametered possibly
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

	void set_score(Score& s) {
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
		auto casts = board.CastHistory();
		// last one is not relevant as we look for even better solutions
		for (auto i = 0; i < board.CastCount()-1; ++i) {
            b.Cast(casts[i]);
			level_score_stats_[i].best = score_(b);
		}
	}
	
	void SetSolution(Board sol) {
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
        for (auto p : b.CastCandidates()) {
            b.Cast(p);
			// if cast is empty, board has to be discovered
            if (discovery_.Discover(b)) {
                Derivative st(b_ptr, p, score_(b), b.hash());
                res.push_back(st);
            }
            // somewhere we have to check if it;s finish and if yes we should reduce number of levels
            b.Restore();
        }
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
	Score score_;
    Count millis_{30000};
};
