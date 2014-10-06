//
//  board.h
//  FragileMirrors
//
//  Created by Anton Logunov on 5/7/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __FragileMirrors__board__
#define __FragileMirrors__board__

#include <iostream>
#include <vector>
#include <stack>
<<<<<<< HEAD

#include <ant/core.h>
#include <ant/grid.h>

using namespace ant::grid;
=======
#include <ant>

using namespace ant::d2::grid;
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
using namespace ant;
using namespace std;

struct BoardBase {
    
    const static char MAX_SIZE = 100;
    
    // can't make 0 be nothing cuz of indexing
    const static char kMirRight     = 0;
    const static char kMirLeft      = 1;
    const static char kMirNothing   = 2;
    
    virtual void cast(const Position& p) = 0;
    virtual void restore() = 0;
    Count mirrorsLeft() const {
<<<<<<< HEAD
        return (Count)(size()*size() - mirrorsDestroyed());
=======
        return size()*size() - mirrorsDestroyed();
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
    }
    virtual double filling() { return mirrorsLeft()/(double)(size()*size()); }
    virtual Count mirrorsDestroyed() const = 0;
    virtual bool isDestroyed(Index row, Index col) const = 0;
    virtual char mirror(Index row, Index col) const = 0;
    virtual size_t size() const = 0;
    
    virtual Count rowMirrorsLeft(Index row) const = 0;
    virtual Count colMirrorsLeft(Index col) const = 0;
    
    virtual Count emptyRowCount() const = 0;    
    virtual Count emptyColCount() const = 0;
    
    virtual bool isRowEmpty(Index row) const {
        return rowMirrorsLeft(row) == 0;
    }
    virtual bool isColEmpty(Index col) const {
        return colMirrorsLeft(col) == 0;
    }
    
    vector<Position> candidateCasts() {
        vector<Position> cs;
        size_t sz = size();
        for (auto i = 0; i < sz; ++i) {
            if (!isRowEmpty(i)) {
                cs.emplace_back(i, -1);
                cs.emplace_back(i, sz);
            }
            if (!isColEmpty(i)) {
                cs.emplace_back(-1, i);
                cs.emplace_back(sz, i);
            }
        }
        return cs;
    }
};

ostream& operator<<(ostream& output, const BoardBase& board);


<<<<<<< HEAD


=======
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
struct Board : BoardBase {
    const static Index kHor = 0;
    const static Index kVer = 1;
    
    using Direction = char;
    // can't make 0 be nothing cuz of indexing
    const constexpr static char kDirTop      = 0;
    const constexpr static char kDirBottom   = 1;
    const constexpr static char kDirLeft     = 2;
    const constexpr static char kDirRight    = 3;
    const constexpr static char kDirNothing  = 4;
    
    static constexpr const char kDirOpposite[] = {
        kDirBottom, 
        kDirTop, 
        kDirRight, 
        kDirLeft, 
        kDirNothing
    };
    
    struct Item {
        // here we have 4 pointers // index points to direction
        // top, right, left, bottom
        Position neighbors[4];
        // relative to mirrors
        char mirror;
        
        // takes direction from which ray is coming
        // returns next position
        // direction after reflection
        tuple<Position, char> nextFrom(char dir) {
            using P = tuple<Position, char>;
            switch (mirror) {
            case kMirLeft: {
                switch (dir) {
                case kDirTop:       return P{neighbors[kDirLeft], kDirLeft} ;    
                case kDirBottom:    return P{neighbors[kDirRight], kDirRight};     
                case kDirLeft:      return P{neighbors[kDirTop], kDirTop};   
                case kDirRight:     return P{neighbors[kDirBottom], kDirBottom};  
                default: assert(false);    
                }
            }
            case kMirRight: {
                switch (dir) {
                case kDirTop:       return P{neighbors[kDirRight], kDirRight} ;    
                case kDirBottom:    return P{neighbors[kDirLeft], kDirLeft};     
                case kDirLeft:      return P{neighbors[kDirBottom], kDirBottom};   
                case kDirRight:     return P{neighbors[kDirTop], kDirTop};  
                default: assert(false); 
                }
            }
            default: return P{neighbors[dir], dir};
            }
        }
    };
    
    Board(const vector<string>& board) {
<<<<<<< HEAD
        board_size_ = (Int)board.size()+2;
        destroyed_count_ = 0;
        mirrors_left_[kHor].resize((Count)board.size(), (Count)board.size());
        mirrors_left_[kVer].resize((Count)board.size(), (Count)board.size());
        empty_row_count_ = 0;
        empty_col_count_ = 0;
        
        hash_function_.reset(new ZobristHashing<HASH_BITS_COUNT>({board_size_, board_size_}, 1));
        
        Position ns[4];
        default_random_engine rng;
        uniform_int_distribution<Index> distr(0, numeric_limits<Index>::max()/(size()*size()));
        items_.resize((Count)board.size()+2, (Count)board.size()+2);
        destroyed_items_.resize((Count)board_size_, (Count)board_size_);
        destroyed_items_.fill(false);
        //hashed_items_.resize((Count)board_size_, (Count)board_size_);
        //hashed_items_.fill(0);
=======
        board_size_ = board.size()+2;
        destroyed_count_ = 0;
        mirrors_left_[kHor].resize(board.size(), board.size());
        mirrors_left_[kVer].resize(board.size(), board.size());
        empty_row_count_ = 0;
        empty_col_count_ = 0;
        
        Position ns[4];
        default_random_engine rng;
        uniform_int_distribution<Index> distr(0, numeric_limits<Index>::max()/(size()*size()));
        items_.resize(board.size()+2, board.size()+2);
        destroyed_items_.resize(board_size_, board_size_);
        destroyed_items_.fill(false);
        hashed_items_.resize(board_size_, board_size_);
        hashed_items_.fill(0);
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
        for (auto row = 1; row <= board.size(); ++row) {
            for (auto col = 1; col <= board.size(); ++col) {
                ns[kDirTop]     = {row-1, col};
                ns[kDirBottom]  = {row+1, col};
                ns[kDirLeft]    = {row, col-1};
                ns[kDirRight]   = {row, col+1};
                memcpy(items_(row,col).neighbors, ns, sizeof(Position[4]));
                items_(row,col).mirror = board[row-1][col-1] == 'R'? kMirRight : kMirLeft;
<<<<<<< HEAD
                //hash += hashed_items_(row, col) = distr(rng);
                hash_function_->xorNothing(&hash_); // xor out 
                hash_function_->xorState(&hash_, Position{row, col}, 0);
=======
                hash += hashed_items_(row, col) = distr(rng);
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
            }
        }
        for (auto i = 0; i < board.size()+2; ++i) {
            items_(i, 0).mirror = kMirNothing;
<<<<<<< HEAD
            items_(i, (Count)board.size()+1).mirror = kMirNothing;
            items_(0, i).mirror = kMirNothing;
            items_((Count)board.size()+1, i).mirror = kMirNothing;
            
            items_(i, 0).neighbors[kDirRight] = {i, 1};
            items_(i, (Count)board.size()+1).neighbors[kDirLeft] = {i, static_cast<Int>(board.size())};
            items_(0, i).neighbors[kDirBottom] = {1, i};
            items_((Count)board.size()+1, i).neighbors[kDirTop] = {static_cast<Int>(board.size()), i};
=======
            items_(i, board.size()+1).mirror = kMirNothing;
            items_(0, i).mirror = kMirNothing;
            items_(board.size()+1, i).mirror = kMirNothing;
            
            items_(i, 0).neighbors[kDirRight] = {i, 1};
            items_(i, board.size()+1).neighbors[kDirLeft] = {i, static_cast<Int>(board.size())};
            items_(0, i).neighbors[kDirBottom] = {1, i};
            items_(board.size()+1, i).neighbors[kDirTop] = {static_cast<Int>(board.size()), i};
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
        }
    }
    
    Direction castDirection(const Position& p) {
        if (p.row == 0) {
            return kDirBottom;
        } else if (p.col == 0) {
            return kDirRight;
        } else if (p.row == items_.row_count()-1) {
            return kDirTop;
        } else {
            return kDirLeft;
        }
    }
    
    Count mirrorsDestroyed() const override {
        return destroyed_count_;
    }
    
    bool isDestroyed(Index row, Index col) const override {
        return (bool)destroyed_items_(row+1, col+1);
    }
    
    char mirror(Index row, Index col) const override {
        return items_(row+1, col+1).mirror;
    }
    
    size_t size() const override {
        return board_size_-2;
    }
        
    void cast(const Position& pp) override {
        Position p(pp);
        p.shift(1, 1);
        char dir = castDirection(p);
<<<<<<< HEAD
        //assert(!Rectangle(1, 1, size(), size()).hasInside(p));
=======
        assert(!Rectangle(1, 1, size(), size()).hasInside(p));
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
        tie(p, dir) = items_(p).nextFrom(dir);
        destroyed_.push_back({});
        while (items_(p).mirror != kMirNothing) {
            assert(dir != kDirNothing);
            destroyed_.back().push(p);
            destroy(p);
            tie(p, dir) = items_(p).nextFrom(kDirOpposite[dir]);
        }      
    }
<<<<<<< HEAD
       
    Count tryCast(const Position& pp) {
        static ant::stack<Position> recover_stack;
=======
    
    /*
     everything should be a cast
    
     int res = 0;
     forn(i, n) {
     res += (cnt[i] == 1) + (cnt[n+i] == 1);
     res += (cnt[i] == 3) + (cnt[n+i] == 3);
     res += (cnt[i] == 5) + (cnt[n+i] == 5);
     res -= ((cnt[i] == 0) + (cnt[n+i] == 0)) * 15;
     }
     res *= ALONE_COEFF;
     forn(i, n) res += cnt[i];
     int KC = 7;
     forn(i, KC) res += cnt[n/2+KC/2-i] + cnt[n+n/2+KC/2-i];
     return res;
     
     
     const double ALONE_COEFF = 1.75;

    */
    
    ant::stack<Position> recover_stack;
    Count tryCast(const Position& pp) {
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
        // stack is cleared.. by nature
        Position p(pp);
        p.shift(1, 1);
        char dir = castDirection(p);
        tie(p, dir) = items_(p).nextFrom(dir);
        Count destroyed_count = destroyed_count_;
        while (items_(p).mirror != kMirNothing) {
            if (destroyed_items_(p)) {
                p = items_(p).neighbors[dir];
                continue;
            }
            destroyed_items_(p) = true;
            --mirrors_left_[kHor][p.row-1];
            --mirrors_left_[kVer][p.col-1];
            recover_stack.push(p);
            ++destroyed_count; 
            tie(p, dir) = items_(p).nextFrom(kDirOpposite[dir]);
        }      
        
        destroyed_count *= 2;
        for (auto& p : recover_stack) {
            destroyed_count += (rowMirrorsLeft(p.row-1) == 0) + (colMirrorsLeft(p.col-1) == 0);
            destroyed_count -= (rowMirrorsLeft(p.row-1) == 1) + (colMirrorsLeft(p.col-1) == 1);
            destroyed_count -= (rowMirrorsLeft(p.row-1) == 3) + (colMirrorsLeft(p.col-1) == 3);
            destroyed_count -= (rowMirrorsLeft(p.row-1) == 5) + (colMirrorsLeft(p.col-1) == 5);
        }
        
        while (!recover_stack.empty()) {
            p = recover_stack.top();
            ++mirrors_left_[kHor][p.row-1];
            ++mirrors_left_[kVer][p.col-1];
            destroyed_items_(p) = false;
            recover_stack.pop();
        }
        return destroyed_count;
    }
        
    void restore() override {
        auto& st = destroyed_.back();
        while (!st.empty()) {
            restore(st.top());
            st.pop();
        }
        destroyed_.pop_back();
    }
    
    void restoreAll() {
        while (!destroyed_.empty()) {
            restore();
        }
    }
    
    bool isRowEmpty(Index row) const override {
        return mirrors_left_[kHor][row] == 0;
    }
    
    bool isColEmpty(Index col) const override {
        return mirrors_left_[kVer][col] == 0;
    }
    
    Count rowMirrorsLeft(Index row) const {
        return mirrors_left_[kHor][row];
    }
    Count colMirrorsLeft(Index col) const {
        return mirrors_left_[kVer][col];
    }
    
    Count emptyRowCount() const override { 
        return empty_row_count_; 
    }
    
    Count emptyColCount() const override {
        return empty_col_count_;
    }
    
    //private:
    void destroy(const Position& p) {
<<<<<<< HEAD
        //assert(Rectangle(1, 1, size(), size()).hasInside(p));
=======
        assert(Rectangle(1, 1, size(), size()).hasInside(p));
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
        if (--mirrors_left_[kHor][p.row-1] == 0) ++empty_row_count_;
        if (--mirrors_left_[kVer][p.col-1] == 0) ++empty_col_count_;
        ++destroyed_count_;
        Item &item = items_(p);
        items_(item.neighbors[kDirTop]).neighbors[kDirBottom] = item.neighbors[kDirBottom];
        items_(item.neighbors[kDirBottom]).neighbors[kDirTop] = item.neighbors[kDirTop];
        items_(item.neighbors[kDirLeft]).neighbors[kDirRight] = item.neighbors[kDirRight];
        items_(item.neighbors[kDirRight]).neighbors[kDirLeft] = item.neighbors[kDirLeft];
        destroyed_items_(p) = true;
<<<<<<< HEAD
        //hash -= hashed_items_(p);
        hash_function_->xorState(&hash_, p, 0); // xor out
        hash_function_->xorNothing(&hash_); // xor in
    }
    
    void restore(const Position& p) {
        //assert(Rectangle(1, 1, size(), size()).hasInside(p));
=======
        hash -= hashed_items_(p);
    }
    
    void restore(const Position& p) {
        assert(Rectangle(1, 1, size(), size()).hasInside(p));
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
        if (mirrors_left_[kHor][p.row-1]++ == 0) --empty_row_count_;
        if (mirrors_left_[kVer][p.col-1]++ == 0) --empty_col_count_;
        --destroyed_count_;
        Item& item = items_(p);
        items_(item.neighbors[kDirTop]).neighbors[kDirBottom] = p;
        items_(item.neighbors[kDirBottom]).neighbors[kDirTop] = p;
        items_(item.neighbors[kDirLeft]).neighbors[kDirRight] = p;
        items_(item.neighbors[kDirRight]).neighbors[kDirLeft] = p;
        destroyed_items_(p) = false;
<<<<<<< HEAD
        //hash += hashed_items_(p);
        hash_function_->xorNothing(&hash_); // xor out
        hash_function_->xorState(&hash_, p, 0); // xor in
    }
    
    bool operator==(const Board& b) const {
        return hash_ == b.hash_;
        //return hash == b.hash;
    }
    
private:
    //Index hash;
=======
        hash += hashed_items_(p);
    }
    
    bool operator==(const Board& b) const {
        return hash == b.hash;
    }
    
    Index hash;
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
    Count destroyed_count_;
    vector<ant::stack<Position>> destroyed_; 
    vector<Count> mirrors_left_[2];
    Count empty_row_count_;
    Count empty_col_count_;
    // here 0 and last indices will be placed for 
    // special elements that will keep next element 
    // to destroy or just opposite entry
<<<<<<< HEAD
    //Grid<Index> hashed_items_;
    Grid<Item> items_;
    Grid<char> destroyed_items_;
    Int board_size_;
    
    constexpr static size_t HASH_BITS_COUNT = 64;
    shared_ptr<ZobristHashing<HASH_BITS_COUNT>> hash_function_;
    ZobristHashing<HASH_BITS_COUNT>::value hash_;
=======
    Grid<Index> hashed_items_;
    Grid<Item> items_;
    Grid<char> destroyed_items_;
    size_t board_size_;
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250

};

#endif /* defined(__FragileMirrors__board__) */












