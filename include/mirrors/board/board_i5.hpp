#pragma once
#include <functional>
#include "mirrors/common/grid.hpp"
#include "mirrors/common/origin_grid.hpp"
#include "mirrors/common/zobrist_hashing.hpp"
#include "mirrors/common/cast_node.hpp"
#include "mirrors/board/board_i1_util.hpp"


namespace mirrors {

class Board_i5 {
    using BoardGrid = OriginGrid<Grid<Mirror>>;
    using CastNode_ = CastNode<Position>;
    static constexpr board_size_t kBorderSize = 2;

public:
    explicit Board_i5(const Grid<Mirror>& mirrors_param) :
            mirrors(MakeBoardGrid(mirrors_param)),
            col_mirror_count(mirrors_param.size(), mirrors_param.size()),
            row_mirror_count(col_mirror_count),
            hashing_(std::make_shared<ZobristHashing>(mirrors_param.size())) {}

    static BoardGrid MakeBoardGrid(const Grid<Mirror>& mirrors_param) {
        BoardGrid bg{{-1, -1}, mirrors_param.size() + kBorderSize};
        bg.grid().fill(Mirror::Border);
        for (auto row = 0; row < mirrors_param.size(); ++row) {
            for (auto col = 0; col < mirrors_param.size(); ++col) {
                bg[{row, col}] = mirrors_param(row, col);
            }
        }
        return bg;
    }

    bool AllDestroyed() const {
        return destroyed_count_ == static_cast<cell_count_t>(size())*size();
    }

    void ForEachCastCandidate(const std::function<void(const Position &)>& func) const {
        for (board_size_t i = 0; i < size(); ++i) {
            if (row_mirror_count[i] != 0) {
                func({i, -1});
                func({i, size()});
            }
            if (col_mirror_count[i] != 0) {
                func({-1, i});
                func({size(), i});
            }
        }
    }

    hash_value_t hash() const {
        return hash_;
    }

    board_size_t size() const {
        return mirrors.grid().size()-kBorderSize;
    }

    cell_count_t destroyed_count() const {
        return destroyed_count_;
    }

    board_size_t empty_cols() const {
        return empty_cols_;
    }

    board_size_t empty_rows() const {
        return empty_rows_;
    }

    std::vector<Position> cast_history() const {
        return CastNode_::ToHistory(cast_node);
    }
private:
    OriginGrid<Grid<Mirror>> mirrors;
    cell_count_t destroyed_count_ {};
    board_size_t empty_cols_ {};
    board_size_t empty_rows_ {};
    std::vector<board_size_t> col_mirror_count;
    std::vector<board_size_t> row_mirror_count;

    hash_value_t hash_ = ZobristHashing::NOTHING;
    std::shared_ptr<ZobristHashing> hashing_;
    std::shared_ptr<CastNode_> cast_node {};

    friend struct Cast_i1;
};

}