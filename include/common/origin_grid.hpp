#pragma once
#include "common/position.hpp"
#include "grid.hpp"


namespace mirrors {

template<class G>
class OriginGrid {
public:
    using value_type = typename G::value_type;

    OriginGrid() {}
    OriginGrid(Position origin, board_size_t size) : origin_(origin), grid_(size) {}

    value_type& operator()(const Position& p) {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }
    const value_type& operator()(const Position& p) const {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }

    value_type& operator[](const Position& p) {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }
    const value_type& operator[](const Position& p) const {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }


    G& grid() {
        return grid_;
    }

    const G& grid() const {
        return grid_;
    }

    void set_grid(const G& grid) {
        grid_ = grid;
    }

    void set_origin(const Position& p) {
        origin_ = p;
    }

    Position& origin() {
        return origin_;
    }
    const Position origin() const {
        return origin_;
    }

private:
    Position origin_;
    G grid_;
};

}
