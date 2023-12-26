#pragma once
#include "mirrors/common/zobrist_hashing.hpp"
#include "mirrors/common/cast_node.hpp"
#include "mirrors/board/board_i1.hpp"


namespace mirrors {

// Adds hashing.
class Board_i2: public board::Board_i1 {
    using CastNode_ = CastNode<Position>;
public:
    explicit Board_i2(const Grid<Mirror>& mirrors_param) : Board_i1(mirrors_param) {
        hashing_ = std::make_shared<ZobristHashing>(mirrors_param.size());
    }

    void OnCast(const Position& pos) override {
        cast_node = std::make_shared<CastNode_>(pos, cast_node);
    }

    void OnMirrorDestroyed(const Position& pos) override {
        hashing_->xorState(&hash_, pos);
    }

    hash_value_t hash() const {
        return hash_;
    }

    std::vector<Position> cast_history() const {
        return CastNode_::ToHistory(cast_node);
    }

private:
    hash_value_t hash_ = ZobristHashing::NOTHING;
    std::shared_ptr<ZobristHashing> hashing_;
    std::shared_ptr<CastNode_> cast_node {};
};

}