//
//  ant.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/10/14.
//
//

#ifndef FRAGILE_MIRRORS_ant_hpp
#define FRAGILE_MIRRORS_ant_hpp

namespace ant {

typedef size_t Count;
typedef size_t Index;
typedef int    Int;
typedef double Float;
  
    
// make it possible to substruct
struct Indent {
    Indent() : Indent(0, 0) {}
    Indent(Int row, Int col) : row(row), col(col) {}
    
    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }
    
    Int area() const {
        return row*col;
    }
    
    Int row, col;
};
        
struct Size {
    Size() : row(0), col(0) {}
    Size(Int row, Int col)
    : row(row), col(col) {}
    
    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }
    
    Count cell_count() const {
        return row*col;
    }
    
    Size swapped() const {
        return Size(col, row);
    }
    
    void swap() {
        std::swap(row, col);
    }
    
    Int row, col;
};

bool operator==(const Size& s_0, const Size& s_1);



struct Position {
    // operators see below
    Position() : Position(0, 0) {}
    Position(Int row, Int col) : row(row), col(col) {}
    
    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }
    
    void shift(Int row, Int col) {
        this->row += row;
        this->col += col;
    }
    
    void swap() {
        std::swap(row, col);
    }
    Position swapped() const {
        return Position(col, row);
    }
    
    Int row, col;
    
    struct TopLeftComparator {
        bool operator()(const Position& p_0, const Position& p_1) {
            return p_0.row < p_1.row || (p_0.row == p_1.row && p_0.col < p_1.col);
        }
    };
    struct BottomRightComparator {
        bool operator()(const Position& p_0, const Position& p_1) {
            return p_0.row > p_1.row || (p_0.row == p_1.row && p_0.col > p_1.col);
        }
    };
};   

Position operator-(const Position& p, const Indent& n);
Position operator+(const Position& p, const Indent& n);        
bool operator==(const Position& p_0, const Position& p_1);        
bool operator!=(const Position& p_0, const Position& p_1);



template<class T>
struct Grid {
    struct const_iterator : std::iterator<std::random_access_iterator_tag, T> {
        
        const_iterator(const Grid<T>& grid, Index index = 0) : grid_(grid), index_(index) {}
        const T& operator*() const { 
            return grid_.grid_[index_]; 
        } 
        bool operator==(const const_iterator& it) {
            return index_ == it.index_; 
        }
        bool operator!=(const const_iterator& it) {
            return index_ != it.index_;
        }
        
        // pred
        const_iterator& operator++() {
            ++index_;
            return *this;
        }
        // post
        const_iterator operator++(int) { 
            const_iterator it(grid_, index_);
            ++index_; 
            return it;
        }
        
    protected:
        const Grid<T>& grid() {
            return grid_;
        }
        
        const Grid<T>& grid_;
        Index index_;
    };
    
    struct iterator : const_iterator {
        using const_iterator::index_;
        using const_iterator::grid_;
        
        iterator(Grid<T>& grid, Index index = 0) : const_iterator(grid, index) {}
        
        iterator& operator++() {
            return static_cast<iterator&>(const_iterator::operator++());
        }
        iterator operator++(int) {
            iterator it(grid(), index_); 
            ++index_;
            return it;
        }
        
        T& operator*() { 
            return grid().grid_[index_]; 
        } 
        
    private:
        Grid<T>& grid() {
            return const_cast<Grid<T>&>(const_iterator::grid_);
        }
    };
    
    const_iterator begin() const {
        return const_iterator(*this);
    }
    iterator begin() {
        return iterator(*this);
    }
    const_iterator end() const {
        return const_iterator(*this, cell_count());
    }
    iterator end() {
        return iterator(*this, cell_count());
    }
    
    
    Grid() : Grid(0, 0) {}
    Grid(Count row_count, Count col_count)
    :   row_count_(row_count), 
    col_count_(col_count), 
    grid_(row_count*col_count) {
    }
    Grid(const Size& size) : Grid(size.row, size.col) {}
    Grid(std::initializer_list<std::vector<T>> list)
    :   Grid(list.size(), list.size() == 0 ? 0 : list.begin()->size()){
        auto b = list.begin();
        for (auto r = 0; r < row_count(); ++r, ++b) {
            std::copy(b->begin(), b->end(), grid_.begin() + r*col_count());
        }
    }
    
    bool isInside(const Position& p) {
        return isInside(p.row, p.col);
    }
    bool isInside(Int row, Int col) {
        return row >= 0 && row < row_count_ && 
        col >= 0 && col < col_count_;
    }
    
    void resize(Count row_count, Count col_count) {
        row_count_ = row_count;
        col_count_ = col_count;
        grid_.resize(row_count*col_count);
    }
    void resize(const Size& size) {
        resize(size.row, size.col);
    }
    
    void fill(const T& t) {
        std::fill(grid_.begin(), grid_.end(), t);
    }
    
    Count row_count() const { return row_count_; }
    Count col_count() const { return col_count_; }
    Size size() const { return Size(row_count(), col_count()); } 
    Count cell_count() const { return row_count()*col_count(); } 
    
    T& operator()(const Position& p) {
        return grid_[p.row*col_count_ + p.col];
    }
    const T& operator()(const Position& p) const {
        return grid_[p.row*col_count_ + p.col];
    }
    
    T& operator[](const Position& p) {
        return grid_[p.row*col_count_ + p.col];
    }
    const T& operator[](const Position& p) const {
        return grid_[p.row*col_count_ + p.col];
    }
    
    T& operator()(Int row, Int col) {
        return grid_[row*col_count_ + col];
    }
    const T& operator()(Int row, Int col) const {
        return grid_[row*col_count_ + col];
    }
private:
    Count row_count_, col_count_;
    std::vector<T> grid_;
    
    friend class const_iterator;
    template<class U>
    friend bool operator==(const Grid<U>& g_0, const Grid<U>& g_1);
};

} // end namespace ant 


#endif
