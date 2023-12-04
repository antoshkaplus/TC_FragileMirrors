#pragma once
#include "common/zobrist_hashing.hpp"
#include "common/cast_node.hpp"
#include "board/board_i1.hpp"


namespace mirrors::board {

// Adds hashing.
class Board_i2: public Board_i1 {
    using CaseNode = CastNode<Position>;
public:
    explicit Board_i2(const Grid<Mirror>& mirrors_param) : Board_i1(mirrors_param) {
        hashing_ = std::make_shared<ZobristHashing>(mirrors_param.size());
    }

    void OnCast(const Position& pos) override {
        cast_node = std::make_shared<CaseNode>(pos, cast_node);
    }

    void OnMirrorDestroyed(const Position& pos) override {
        hashing_->xorState(&hash_, pos);
    }

    hash_value_t hash() const {
        return hash_;
    }

    std::vector<Position> cast_history() const {
        return CaseNode::ToHistory(cast_node);
    }

private:
    hash_value_t hash_ = ZobristHashing::NOTHING;
    std::shared_ptr<ZobristHashing> hashing_;
    std::shared_ptr<CaseNode> cast_node {};
};

}