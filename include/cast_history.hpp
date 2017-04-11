//
// Created by Anton Logunov on 4/8/17.
//
#pragma once

#include "util.hpp"

class CastHistory {
public:
    virtual void Push(const Position& p) = 0;
    virtual void Pop() = 0;
    virtual Count Count() const = 0;
};


class CastHistory_Vector : CastHistory {
public:
    template<class Functor>
    void ForEach(Functor func) {

    }

    void Push(const Position& p) override {
        history_.push_back(p);
    }

    void Pop() override {
        history_.pop_back();
    }

    ::Count Count() const override {
        return history_.size();
    }

private:
    vector<Position> history_;

    friend vector<Position> ToVector(const CastHistory_Vector& history);
};


class CastHistory_Nodes : CastHistory {
public:
    template<class Functor>
    void ForEach(Functor func) {

    }

    void Pop() override {
        history_ = history_->previous;
        --count_;
    }

    void Push(const Position& p) override {
        CastNode* new_cast = new CastNode(p, history_);
        history_.reset(new_cast);
        ++count_;
    }

    ::Count Count() const override {
        return count_;
    }

private:
    shared_ptr<CastNode> history_;
    ::Count count_ = 0;

    friend vector<Position> ToVector(const CastHistory_Nodes& history);
};

vector<Position> ToVector(const CastHistory_Nodes& history);
vector<Position> ToVector(const CastHistory_Vector& history);