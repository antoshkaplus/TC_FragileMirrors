//
// Created by Anton Logunov on 3/31/17.
//

#pragma once

#include <experimental/optional>

#include "util.hpp"

#include "board_v1.hpp"
#include "score.hpp"


class DFS {
    using Board = Board_v1;
    using Score = InterLevelScoreFunctor<Board>;


    struct State {
        shared_ptr<const Board> board;
        Position cast;
        double score;
        Board::HashType hash;

        State() {}
        State(shared_ptr<const Board> board, Position cast, double score, Board::HashType hash)
                : board(board), cast(cast), score(score), hash(hash) {}

        bool operator<(const State& s) const {
            return score < s.score;
        }
    };

public:

    Board Destroy(const Board& board) {

        auto minCastCount = numeric_limits<int>::max();
        auto res = board;

        auto startMillisCount = GetMillisCount();

        // dfs
        while (GetMillisCount() - startMillisCount < millis_timeout_) {

            Board b;
            if (queue_.empty()) {
                b = board;
            } else {
                auto st = Pop();

                b = *st.board;
                b.Cast(st.cast);
            }

            while (!b.AllDestroyed() && b.CastCount() < minCastCount) {
                auto bestCast = IntroduceDerivatives(b);
                if (!bestCast) break;
                b.Cast(*bestCast);
            }

            if (b.AllDestroyed() && b.CastCount() < minCastCount) {
                res = b;
            }
        }
        return res;
    }

    void set_timeout(unsigned millis) {
        millis_timeout_ = millis;
    }

    void set_score(Score& s) {
        score_ = s;
    }

    void set_queue_cap(Count cap) {
        queue_cap_ = cap;
    }

private:

    experimental::optional<Position> IntroduceDerivatives(Board& b) {
        assert(!b.AllDestroyed());
        auto b_ptr = make_shared<Board>(b);
        State best;
        best.score = numeric_limits<double>::min();
        for (auto p : b.CastCandidates()) {
            b.Cast(p);
            if (discovered_.count(b.hash()) == 0) {
                State st(b_ptr, p, score_(b), b.hash());
                if (best.score == numeric_limits<double>::min()) {
                    best = st;
                } else {
                    if (best.score < st.score) {
                        swap(best, st);
                    }
                    Insert(st);
                }
            }
            b.Restore();
            assert(!b.AllDestroyed());
        }
        if (best.score == numeric_limits<double>::min()) {
            return {};
        }
        return {best.cast};
    }

    State Pop() {
        auto it = prev(queue_.end());
        State s = *it;
        queue_.erase(it);
        return s;
    }

    void Insert(const State st) {
        if (queue_.size() == queue_cap_) {
            auto it = queue_.begin();
            if (it->score < st.score) {
                queue_.erase(it);
                discovered_.erase(it->hash);
            }
        }
        queue_.insert(st);
        discovered_.insert(st.hash);
    }

    // by default queue is max
    unsigned millis_timeout_ = 10000;
    Score score_;

    multiset<State> queue_;
    Count queue_cap_ = 1000000;
    unordered_set<Board::HashType> discovered_;

};