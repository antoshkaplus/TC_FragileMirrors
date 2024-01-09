//
// Created by Anton Logunov on 4/8/17.
//
#pragma once

#include "util.hpp"

class CastHistory {
public:
    virtual void Push(const Position& p) = 0;
    virtual void Pop() = 0;
    virtual ant::Count Count() const = 0;
};


class CastHistory_Vector : CastHistory {
public:
    template<class Functor>
    void ForEach(Functor func) {
        throw runtime_error("not supported");
    }

    void Push(const Position& p) override {
        history_.push_back(p);
    }

    void Pop() override {
        history_.pop_back();
    }

    ant::Count Count() const override {
        return history_.size();
    }

private:
    vector<Position> history_;

    friend vector<Position> ToVector(const CastHistory_Vector& history);
};

// I want to add more data in there
class CastHistory_Nodes : CastHistory {
public:
    template<class Functor>
    void ForEach(Functor func) {
        throw runtime_error("not supported");
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

class CastHistory_Nodes_v2 {

    struct Data {
        Position pos;
        short ray_index;
    };

    using Node = TrailNode<Data>;
    using NodePtr = TrailNodePtr<Data>;

public:
    void Pop() {
        history_ = history_->previous;
        --count_;
    }

    void Push(const Position&p, short ray_index) {
        Node *new_node = new Node(Data{p, ray_index}, history_);
        history_.reset(new_node);
        ++count_;
    }

    ::Count Count() const {
        return count_;
    }

    NodePtr history_;
    ::Count count_{0};

    friend vector<Position> ToVector(const CastHistory_Nodes_v2& history);
    friend vector<short> ToRayVector(const CastHistory_Nodes_v2& history);
};



vector<Position> ToVector(const CastHistory_Vector& history);

vector<Position> ToVector(const CastHistory_Nodes_v2& history);
vector<short> ToRayVector(const CastHistory_Nodes_v2& history);
