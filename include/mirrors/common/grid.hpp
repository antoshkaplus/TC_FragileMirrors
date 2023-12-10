#pragma once
#include <vector>
#include "mirrors/common/position.hpp"


namespace mirrors {

template<class T>
struct Grid {
    // Allows clients to know it value type.
    using value_type = T;

    typename std::vector<T>::const_iterator begin() const {
        return grid_.begin();
    }

    typename std::vector<T>::iterator begin() {
        return grid_.begin();
    }

    typename std::vector<T>::const_iterator end() const {
        return grid_.end();
    }

    typename std::vector<T>::iterator end() {
        return grid_.end();
    }

    Grid() : Grid(0) {}

    Grid(board_size_t size)
            : size_(size),
              grid_(cell_count()) {}

    Grid(board_size_t size, const T &value)
            : size_(size),
              grid_(cell_count(), value) {}

    void fill(const T &t) {
        std::fill(grid_.begin(), grid_.end(), t);
    }

    board_size_t size() const { return size_; }
    cell_count_t cell_count() const { return static_cast<cell_count_t>(size()) * size(); }

    T &operator[](const Position &p) {
        return grid_[index(p)];
    }

    const T &operator[](const Position &p) const {
        return grid_[index(p)];
    }

    T &operator()(board_size_t row, board_size_t col) {
        return grid_[index({row, col})];
    }

    const T &operator()(board_size_t row, board_size_t col) const {
        return grid_[index({row, col})];
    }

private:
    cell_count_t index(const Position &p) const {
        return static_cast<cell_count_t>(p.row) * size_ + p.col;
    }

    board_size_t size_;
    std::vector<T> grid_;

    friend struct const_iterator;

    template<class U>
    friend bool operator==(const Grid<U> &g_0, const Grid<U> &g_1);
};

template<class T>
bool operator==(const Grid<T> &g_0, const Grid<T> &g_1) {
    return g_0.row_count_ == g_1.row_count_ && g_0.grid_ == g_1.grid_;
}

template<class T>
bool operator!=(const Grid<T> &g_0, const Grid<T> &g_1) {
    return !(g_0 == g_1);
}

}