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



template<class Func>
static void ForEach(const CastHistory_Nodes_v2& history, Func func) {
    auto node = history.history_.get();
    while (node != nullptr) {
        func(node->value);
        node = node->previous.get();
    }
}

vector<short> ToRayVector(const CastHistory_Nodes_v2& history) {
    vector<short> casts;
    ForEach(history, [&](const auto& val){
        casts.push_back(val.ray_index);
    });
    reverse(casts.begin(), casts.end());
    return casts;
}

vector<Position> ToVector(const CastHistory_Nodes_v2& history) {
    vector<Position> casts;
    ForEach(history, [&](const auto& val){
        casts.push_back(val.pos);
    });
    reverse(casts.begin(), casts.end());
    return casts;
}