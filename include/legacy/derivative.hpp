//
// Created by Anton Logunov on 4/12/17.
//
#pragma once

#include "util.hpp"
#include "board.hpp"


template <class Board>
struct Derivative {

    using CastType = typename Board::CastType;
    using HashType = typename Board::HashType;

    shared_ptr<const Board> board;
    CastType cast;
    double score;
    HashType hash;

    Derivative() {}
    Derivative(shared_ptr<const Board> board, CastType cast, double score, HashType hash)
            : board(board), cast(cast), score(score), hash(hash) {}

    bool operator<(const Derivative& s) const {
        return score < s.score;
    }

    bool operator>(const Derivative& s) const {
        return score > s.score;
    }
};


// it's a little bit different
template <class Derivative>
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
template <class Derivative>
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

