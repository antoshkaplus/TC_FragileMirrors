//
// Created by Anton Logunov on 4/3/17.
//
#pragma once

#include "util.hpp"


template<class Board, class Score>
class BeamSearchHistory {

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

    using BeamLevel = vector<Board>;

public:

    Board Destroy(const Board& b_in) {
        unordered_set<HashType> visited;
        vector<Derivative> derivs;
        Count side_count = 4;
        derivs.reserve(beam_width_*side_count*b_in.size());
        MakeNewBeamLevel();
        cur_beam_level().push_back(b_in);
        while (true) {
            MakeNewBeamLevel();
            for (auto& b : prev_beam_level()) {
                Count d_was = b.MirrorsDestroyed();
                for (auto& c : b.CastCandidates()) {
                    b.Cast(c);
                    Count d_now = b.MirrorsDestroyed();
                    if (d_now > d_was && visited.count(b.hash()) == 0) {
                        visited.insert(b.hash());
                        derivs.emplace_back(&b, c, b.hash(), score_(b));
                    }
                    b.Restore();
                }
            }
            Count sz = min<Count>(beam_width_, derivs.size());
            nth_element(derivs.begin(), derivs.begin()+sz-1, derivs.end());
            derivs.resize(sz);
            cur_beam_level().resize(sz);
            for (Index i = 0; i < sz; ++i) {
                derivs[i].origin->Cast(derivs[i].cast);
                cur_beam_level()[i] = *(derivs[i].origin);
                derivs[i].origin->Restore();
            }
            auto rr = max_element(cur_beam_level().begin(), cur_beam_level().end(), [] (const Board& b_0, const Board& b_1) {
                return b_0.MirrorsDestroyed() < b_1.MirrorsDestroyed();
            });
            if (rr->AllDestroyed()) {
                return *rr;
            }
            /// cleanup before next step
            derivs.clear();
            visited.clear();
        }
    }

    void set_score(Score score) {
        score_ = score;
    }

    void set_beam_width(Count beam_width) {
        beam_width_ = beam_width;
    }

    const vector<BeamLevel>& beam_levels() const {
        return beam_levels_;
    }

    double score(const Board& b) {
        return score_(b);
    }

private:

    void MakeNewBeamLevel() {
        beam_levels_.resize(beam_levels_.size()+1);
        beam_levels_.back().reserve(beam_width_);
    }

    BeamLevel& cur_beam_level() {
        return beam_levels_.back();
    }

    BeamLevel& prev_beam_level() {
        return beam_levels_.at(beam_levels_.size()-2);
    }

    Count beam_width_;
    Score score_;
    vector<BeamLevel> beam_levels_;
};
