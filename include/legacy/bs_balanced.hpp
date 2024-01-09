#pragma once

#include "util.hpp"
#include "board.hpp"
#include "score.hpp"


template<class BoardType>
struct Balancer_1 {

    Balancer_1(int bSize, int beamWidth) {

    }

private:
    int computeLayerProblemSize(const vector<BoardType>& bs) {
        return accumulate(bs.begin(), bs.end(), 0, [](int s, const BoardType& b) {
            return s + b.MirrorsLeft();
        });
    }

public:

    int nextBeamWidth(const vector<BoardType>& curBs, int allowedSize) {
        double aveSize = computeLayerProblemSize(curBs) / curBs.size();
        return allowedSize / aveSize;
    }

};

template<class BoardType>
struct Balancer_2 {

    Balancer_2(int bSize, int beamWidth) {
        initialTotal = beamWidth * bSize * bSize*4;
    }

private:
    int compute(const BoardType& b) {
        return b.RayCount() * sqrt(b.MirrorsLeft());
    }

    int computeCurrentTotal(const vector<BoardType>& bs) {
        return accumulate(bs.begin(), bs.end(), 0, [&](int init, const BoardType& b) {
            return init + compute(b);
        });
    }

public:
    int nextBeamWidth(const vector<BoardType>& bs) {
        return initialTotal * bs.size() / computeCurrentTotal(bs);
    }

private:
    int initialTotal;
};
// has to keep ScoreType as template parameter to support
// score functions that require specific board class as argument
template<class BoardType, class ScoreType>
class BeamSearchBalanced {

    using HashType = typename BoardType::HashType;
    using CastType = typename BoardType::CastType;

    /// can make use of more parametered possibly
    struct Derivative {
        Derivative() {}
        Derivative(BoardType* b, const CastType& p, HashType h, double s)
        : origin(b), cast(p), hash(h), score(s) {}

        BoardType* origin;
        CastType cast;
        HashType hash;
        double score;

        /// want to sort in reverse order
        bool operator<(const Derivative& d) const {
            return score > d.score;
        }
    };


public:


    BoardType Destroy(const BoardType& b_in) {
        Balancer_2<Board_v6> balancer(b_in.size(), beam_width_);

        unordered_set<HashType> visited;
        vector<Derivative> derivs;
        vector<BoardType> b_0;
        vector<BoardType> b_1;
        b_0.reserve(beam_width_);
        b_1.reserve(beam_width_);
        Count side_count = 4;
        derivs.reserve(beam_width_*side_count*b_in.size());
        auto cur = &b_0;
        auto next = &b_1;
        cur->push_back(b_in);
        Timer timer{std::chrono::duration_cast<std::chrono::milliseconds>(time_).count()};
        while (!timer.timeout()) {
            for (auto& b : *cur) {
                Count d_was = b.MirrorsDestroyed();
                auto func = [&](CastType c) {
                    Count d_now = b.MirrorsDestroyed();
                    if (d_now > d_was && visited.count(b.hash()) == 0) {
                        visited.insert(b.hash());
                        derivs.emplace_back(&b, c, b.hash(), score_(b));
                    }
                };
                b.ForEachAppliedCast(func);
            }

            // time to pick amount for the next layer
            Count sz = min<Count>(balancer.nextBeamWidth(*cur), derivs.size());
            nth_element(derivs.begin(), derivs.begin()+sz-1, derivs.end());
            derivs.resize(sz);
            next->resize(sz);
            for (Index i = 0; i < sz; ++i) {
                (*next)[i] = *(derivs[i].origin);
                (*next)[i].Cast(derivs[i].cast);
            }
            swap(cur, next);
            auto rr = max_element(cur->begin(), cur->end(), [] (const BoardType& b_0, const BoardType& b_1) {
                return b_0.MirrorsDestroyed() < b_1.MirrorsDestroyed();
            });
            if (rr->AllDestroyed()) {
                return *rr;
            }
            /// cleanup before next step
            next->clear();
            derivs.clear();
            visited.clear();
        }
        return b_in;
    }

    void set_score(ScoreType score) {
        score_ = score;
    }

    void set_beam_width(Count beam_width) {
        beam_width_ = beam_width;
    }

    void set_time(std::chrono::seconds time) {
        time_ = time;
    }

    Count beam_width_;
    ScoreType score_;
    std::chrono::seconds time_{30};
};
