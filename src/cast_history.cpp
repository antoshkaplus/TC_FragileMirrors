//
// Created by Anton Logunov on 4/10/17.
//
#include "cast_history.hpp"

vector<Position> ToVector(const CastHistory_Vector& history) {
    return history.history_;
}

vector<Position> ToVector(const CastHistory_Nodes& history) {
    vector<Position> casts;
    CastNode* node = history.history_.get();
    while (node != nullptr) {
        casts.push_back(node->cast);
        node = node->previous.get();
    }
    reverse(casts.begin(), casts.end());
    return casts;
}