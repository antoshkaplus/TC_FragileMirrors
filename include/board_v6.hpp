//
//  board_v6.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 1/8/15.
//
//
#pragma once

#include "board_common.hpp"

class Board_v6 : public Board_v2 {
private:
    
    using int8_t = short;
    
    constexpr static size_t HashBitsCount = 64;
    
    const constexpr static int kDirTop      = 0;
    const constexpr static int kDirBottom   = 1;
    const constexpr static int kDirLeft     = 2;
    const constexpr static int kDirRight    = 3;
    const constexpr static int kDirNothing  = 4;
    
    const constexpr static char kMirRight     = 0;
    const constexpr static char kMirLeft      = 1;
    const constexpr static char kMirBorder    = 2;
    const constexpr static char kMirOffset    = 3;
    
    const constexpr static char kOrientHor = 0;
    const constexpr static char kOrientVer = 1;
    
    using Direction = char;
    using Mirror = char;
    using Neighbors = array<short, 4>;
    
    using HashFunction = ZobristHashing<HashBitsCount>;
    using Mirrors = Grid<int8_t>;
public:
    using HashType = typename HashFunction::value;
    
private:
    
    struct Ray {
        Ray(short pos, Direction dir) 
        : pos(pos), dir(dir) {}
        
        short pos;
        Direction dir; 
    };    
    
    struct Item {
        array<short, 4> ns;
        char row;
        char col; 
    };
    
    constexpr const static array<int, 5> kDirOpposite = { {
        kDirBottom, 
        kDirTop, 
        kDirRight, 
        kDirLeft, 
        kDirNothing
    } };
    
    // first index mirror type
    // second index where ray going
    // result direction where will go  
    constexpr const static array<array<char, 4>, 2> kDirReflection = { {
        // kMirRight
        { {
            kDirLeft,  // to top
            kDirRight,   // to bottom
            kDirTop, // to left
            kDirBottom     // to right
        } },
        // kMirLeft
        { {
            kDirRight,   // to top
            kDirLeft,  // to bottom
            kDirBottom,    // to left
            kDirTop  // to right
        } }
    } };
    

public:
    
    Board_v6() {}
    
    Board_v6(const vector<string>& str_board) : board_size_(str_board.size()),
                                                hash_(board_size_) {
        mirrors_destroyed_ = 0;
        empty_lines_count_ = 0;
        
        filled_space_ = str_board.size()*str_board.size();
        empty_space_ = 0;
        
        InitItems();
        mirrors_left_[kOrientHor].resize(board_size_, board_size_);
        mirrors_left_[kOrientVer].resize(board_size_, board_size_);
        InitHash();
        
        InitMirrors(str_board);
        buffer_.reset(new vector<short>());
    }
    
private:
    
    void InitItems() {
        items_.resize(4*board_size_ + board_size_*board_size_);
        ray_direction_.resize(4*board_size_);
        
        // initializing inner links
        auto offset = 4*board_size_;
        auto ToIndex = [&](int r, int c) {
            return r * board_size_ + c + offset;
        };
        for (int r = 0; r < board_size_; ++r) {
            for (int c = 0; c < board_size_; ++c) {
                Index i = ToIndex(r, c);
                auto& t = items_[i];
                t.ns[kDirTop] = ToIndex(r-1, c);
                t.ns[kDirRight] = ToIndex(r, c+1);
                t.ns[kDirBottom] = ToIndex(r+1, c);
                t.ns[kDirLeft] = ToIndex(r, c-1);
                t.row = r;
                t.col = c;
            }
        }
        // initializing border links
        for (int i = 0; i < board_size_; ++i) {
            int s = 4*i;
            int m_i; // mirror index
            int b_i; // border index
            
            // TOP
            m_i = ToIndex(0, i);
            b_i = s + kDirTop;
            
            items_[b_i].ns.fill(-1);
            items_[b_i].ns[kDirBottom] = m_i;
            items_[b_i].row = -1;
            items_[b_i].col = i;
            ray_direction_[b_i] = kDirBottom;
            items_[m_i].ns[kDirTop] = b_i;
            
            
            // RIGHT
            m_i = ToIndex(i, board_size_-1);
            b_i = s + kDirRight;
            items_[b_i].ns.fill(-1);
            items_[b_i].ns[kDirLeft] = m_i;
            items_[b_i].row = i;
            items_[b_i].col = board_size_;
            ray_direction_[b_i] = kDirLeft;
            items_[m_i].ns[kDirRight] = b_i;
            
            // BOTTOM
            m_i = ToIndex(board_size_-1, i);
            b_i = s + kDirBottom;
            items_[b_i].ns.fill(-1);
            items_[b_i].ns[kDirTop] = m_i;
            items_[b_i].row = board_size_;
            items_[b_i].col = i;
            ray_direction_[b_i] = kDirTop;
            items_[m_i].ns[kDirBottom] = b_i;
            
            // LEFT
            m_i = ToIndex(i, 0);
            b_i = s + kDirLeft; 
            items_[b_i].ns.fill(-1);
            items_[b_i].ns[kDirRight] = m_i;
            items_[b_i].row = i;
            items_[b_i].col = -1;
            ray_direction_[b_i] = kDirRight;
            items_[m_i].ns[kDirLeft] = b_i;
        }
    }
    
    void InitMirrors(const vector<string>& str_board) {
        mirrors_.reset(new Mirrors(board_size_, board_size_));
        auto& mirs = *mirrors_;
        for (auto r = 0; r < board_size_; ++r) {
            for (auto c = 0; c < board_size_; ++c) {
                mirs(r, c) = IsRightMirror(str_board[r][c]) ? kMirRight : kMirLeft;
            }
        }
    }
    
    void InitHash() {
        for (auto r = 0; r < board_size_; ++r) {
            for (auto c = 0; c < board_size_; ++c) {
                hash_.HashIn(r, c);
            }
        }
    }
    
public:
    
    Count CastRestorable(short ray_index) override {
        auto& last = *buffer_; 
        last.clear();
        auto& mirs = *mirrors_; 
        
        Ray ray{ray_index, ray_direction_[ray_index]};
        ray = NextFromEmpty(ray);
        while (ray.pos >= ray_direction_.size()) {
            char r = items_[ray.pos].row;
            char c = items_[ray.pos].col;
            if (mirs(r, c) >= kMirOffset) {
                ray = NextFromEmpty(ray);
                continue;
            } 
            last.push_back(ray.pos);
            Destroy(r, c);
            ray = NextFromMirror(ray, mirs(r, c));
            mirs(r, c) += kMirOffset;
        }    
        mirrors_destroyed_ += last.size();
        return last.size();
    }
    
    Count Cast(short ray_index) override {
        auto& mirs = *mirrors_;
        history_casts_.Push({items_[ray_index].row, items_[ray_index].col});
        
        Ray ray = NextFromBorder(ray_index);
        Count count = 0;
        while (ray.pos >= ray_direction_.size()) {
            Destroy(items_[ray.pos].row, items_[ray.pos].col);
            DestroyLinks(ray.pos);
            ray = NextFromMirror(ray, mirs(items_[ray.pos].row, items_[ray.pos].col));
            ++count;
        }    
        empty_space_ += count;
        filled_space_ -= count;
        mirrors_destroyed_ += count;
        return count;
    }
    
    void Restore() override {
        auto& last = *buffer_; 
        auto& mirs = *mirrors_;
        
        mirrors_destroyed_ -= last.size();
        while (!last.empty()) {
            char r = items_[last.back()].row;
            char c = items_[last.back()].col;
            mirs(r, c) -= kMirOffset;
            Restore(r, c);
            last.pop_back();
        }
    }
    
    void Destroy(char row, char col) {
        if (--mirrors_left_[kOrientHor][col] == 0) {
            ++empty_lines_count_;
            // empty is not counted as even?????
        } 
        
        if (--mirrors_left_[kOrientVer][row] == 0) {
            ++empty_lines_count_;
        }
        hash_.HashOut({row, col});
    }
    
    void DestroyLinks(short index) {
        auto& ns = items_[index].ns;
        items_[ns[kDirTop]].ns[kDirBottom] = ns[kDirBottom];
        items_[ns[kDirBottom]].ns[kDirTop] = ns[kDirTop];
        items_[ns[kDirLeft]].ns[kDirRight] = ns[kDirRight];
        items_[ns[kDirRight]].ns[kDirLeft] = ns[kDirLeft];
    }
    
    void Restore(char row, char col) {
        if (++mirrors_left_[kOrientHor][col] == 1) {
            --empty_lines_count_;
        }         
        if (++mirrors_left_[kOrientVer][row] == 1) {
            --empty_lines_count_;
        } 
        hash_.HashIn({row, col});
    }
    
    
    void Reduce(vector<short>& shift) {
        Reduce();
        auto& offset = *buffer_;
        for (auto& s : shift) {
            s -= offset[s];
        }
    }
    
    // 4 * Number of items
    void Reduce() {
        auto& offset = *buffer_;
        offset.resize(items_.size());
        auto cur = 0;
        for (auto i = 0; i < ray_direction_.size(); ++i) {
            if (IsEmptyLine(i)) {
                ++cur;
            } 
            offset[i] = cur;
        }
        for (auto i = ray_direction_.size(); i < items_.size(); ++i) {
            if (items_[items_[i].ns[kDirTop]].ns[kDirBottom] != i) {
                ++cur;
            } 
            offset[i] = cur;
        }
        if (offset[0] == 0) {
            short& p = items_[0].ns[ray_direction_[0]];
            p -= offset[p];
        }
        for (auto i = 1; i < ray_direction_.size(); ++i) {
            if (offset[i-1] != offset[i]) {
                // increased cur on i pos: deleted element
                continue;
            }
            short& p = items_[i].ns[ray_direction_[i]];
            p -= offset[p];
            items_[i - offset[i]] = items_[i];
            ray_direction_[i - offset[i]] = ray_direction_[i];
        }
        for (auto i = ray_direction_.size(); i < items_.size(); ++i) {
            if (offset[i-1] != offset[i]) {
                continue;
            }
            auto& ns = items_[i].ns; 
            for (int q = 0; q < 4; ++q) {
                ns[q] -= offset[ns[q]];
            }
            items_[i - offset[i]] = items_[i];
        }
        // now resize both vectors
        auto last = ray_direction_.size()-1;
        ray_direction_.resize(last + 1 - offset[last]);
        last = items_.size()-1;
        items_.resize(last + 1 - offset[last]);
        
        empty_space_ = 0;
        filled_space_ = items_.size() - ray_direction_.size();
    }
    
    bool IsEmptyLine(short ray_index) {
        return NextFromBorder(ray_index).pos < RayCount();
        
    }
    
    bool AllDestroyed() const override {
        return empty_lines_count_ == 2 * board_size_;
    }
    
    Count size() const override {
        return board_size_;
    }
    
    Count RayCount() const override {
        return ray_direction_.size();
    }
    
    Count MirrorsDestroyed() const override {
        return mirrors_destroyed_;
    }
    
    Count EmptyLinesCount() const override {
        return empty_lines_count_;
    }
    
    HashType hash() const override {
        return hash_.hash();
    }
    
    Count EmptySpace() const {
        return empty_space_;
    }
    
    Count FilledSpace() const {
        return filled_space_;
    }

    Count CastCount() const override {
        return history_casts_.Count();
    }

    vector<Position> CastHistory() const override {
        return ToVector(history_casts_);
    }

    unique_ptr<Board> Clone() const override {
        return make_unique<Board_v6>(*this);
    }
    
private:
    
    Ray NextFromMirror(const Ray& ray, char mir) const {
        Direction dir = kDirReflection[mir][ray.dir];
        return {items_[ray.pos].ns[dir], dir};
    }
    
    Ray NextFromBorder(short ray_index) const {
        Direction dir = ray_direction_[ray_index];
        return {items_[ray_index].ns[dir], dir};
    }
    
    // same as for FromBorder
    Ray NextFromEmpty(const Ray& ray) const {
        return {items_[ray.pos].ns[ray.dir], ray.dir};
    }


    Count board_size_;
    Count mirrors_destroyed_;
    Count empty_lines_count_;

    Count filled_space_;
    Count empty_space_;

    BoardHash hash_;

    vector<Item> items_;
    // they are first in items
    // where is ray directed
    vector<Direction> ray_direction_;
    array<vector<char>, 2> mirrors_left_;

    shared_ptr<Mirrors> mirrors_;
    CastHistory_Nodes history_casts_;
    // use for reduce and restore
    shared_ptr<vector<short>> buffer_;

};
