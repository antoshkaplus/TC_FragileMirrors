//
//  board_v3.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/28/14.
//
// Supports all history

#ifndef FRAGILE_MIRRORS_board_v3_hpp
#define FRAGILE_MIRRORS_board_v3_hpp


#include <iostream>
#include <vector>
#include <stack>
#include <array>
#include <type_traits>
#include <tuple>

#include <ant/core/core.hpp>
#include <ant/grid.h>

#include "util.hpp"
#include "board.hpp"

using namespace std;
using namespace ant;
using namespace ant::grid;

//
//class Board_v3 {
//    
//  
//    using Direction = char;
//    using Mirror = char;
//    
//    // dir - where ray going to
//    struct Ray {
//        Ray(short pos, Direction dir)
//        : pos(pos), dir(dir) {} 
//        
//        short pos;
//        Direction dir;
//    };
//    
//    const constexpr static int kDirTop      = 0;
//    const constexpr static int kDirBottom   = 1;
//    const constexpr static int kDirLeft     = 2;
//    const constexpr static int kDirRight    = 3;
//    const constexpr static int kDirNothing  = 4;
//    
//    const constexpr static char kMirRight     = 0;
//    const constexpr static char kMirLeft      = 1;
//    const constexpr static char kMirBorder    = 2;
//    const constexpr static char kMirOffset    = 3;
//    
//    const constexpr static char kOrientHor = 0;
//    const constexpr static char kOrientVer = 1;
//    
//    constexpr const static array<int, 5> kDirOpposite = { {
//        kDirBottom, 
//        kDirTop, 
//        kDirRight, 
//        kDirLeft, 
//        kDirNothing
//    } };
//    
//    // first index mirror type
//    // second index where ray going
//    // result direction where will go  
//    constexpr const static array<array<char, 4>, 2> kDirReflection = { {
//        // kMirRight
//        { {
//            kDirRight,  // from top
//            kDirLeft,   // from bottom
//            kDirBottom, // from left
//            kDirTop     // from right
//        } },
//        // kMirLeft
//        { {
//            kDirLeft,   // from top
//            kDirRight,  // from bottom
//            kDirTop,    // from left
//            kDirBottom  // from right
//        } }
//    } };
//    
//    constexpr static size_t HashBitsCount = 64;
//
//    
//public:    
//    using HashFunction = ZobristHashing<HashBitsCount>;
//    using HashType = typename HashFunction::value;
//    using Neighbors = vector<array<short, 4>>;
//    using Mirrors = vector<char>;
//private:
//    // should initialize only once in constructor probably
//    Neighbors neighbors_;
//    // sum of history_count_
//    Count destroyed_count_;
//    HashType hash_;
//    Int board_size_;
//    
//    shared_ptr<vector<short>> last_cast_;
//    // first index responsible for line horizontal vs vertical
//    // second index responsible for line index
//    
//    array<vector<Count>, 2> mirrors_left_; 
//    Count empty_lines_count_;
//    Count even_mirrors_lines_;
//    
//    // those guys are initialized when initialization done
//    // through vector of strings
//    shared_ptr<Mirrors> mirrors_;
//    // assume board_size without borders
//    shared_ptr<HashFunction> hash_function_;
//    shared_ptr<vector<Ray>> rays_;
//    
//    vector<short> history_casts_;
//    
//    friend class Board_v2;
//
//public:
//    Board_v3() {}
//    
//    Board_v3(const vector<string>& board) : destroyed_count_(0) {
//        board_size_ = board.size() + 2;
//        mirrors_left_[kOrientHor].resize(board_size_, board_size_);
//        mirrors_left_[kOrientVer].resize(board_size_, board_size_);
//        
//        Count expanded_size = board_size_ + 2;
//        Position origin = {-1, -1};
//        Size size = {expanded_size, expanded_size};
//
//        // seeting up mirrors grid
//        hash_function_.reset(new HashFunction({board_size_, board_size_}, 1));
//        // setting up neighbors
//        short board_area = board_size_*board_size_;
//        mirrors_.reset(new Mirrors(board_area));
//        auto& mirs = *mirrors_;
//        for (auto row = 0; row < board_size_; ++row) {
//            for (auto col = 0; col < board_size_; ++col) {
//                mirs(row, col) = IsRightMirror(board[row][col]) ? kMirRight : kMirLeft;
//            }
//        }  
//        // need some adjustments for sides
//        for (auto i = 0; i < board_size_; ++i) {
//            mirs(-1, i) = mirs(board_size_, i) 
//            = mirs(i, -1) = mirs(i, board_size_) = kMirBorder;
//        }
//        
//        InitNeighbors();
//    }
//    
//private:
//    
//    void InitNeighbors() {
//        neighbors_.origin() = {-1, -1};
//        neighbors_.grid().resize({board_size_+2, board_size_+2});
//        for (auto row = 0; row < board_size_; ++row) {
//            for (auto col = 0; col < board_size_; ++col) {
//                auto& ns = neighbors_(row, col);
//                ns[kDirTop]     = row-1;
//                ns[kDirBottom]  = row+1;
//                ns[kDirLeft]    = col-1;
//                ns[kDirRight]   = col+1;
//                HashIn(row, col);
//            }
//        }  
//        for (auto i = 0; i < board_size_; ++i) {
//            neighbors_(-1, i)[kDirBottom] = 0;
//            neighbors_(board_size_, i)[kDirTop] = board_size_-1;
//            neighbors_(i, board_size_)[kDirLeft] = board_size_-1;
//            neighbors_(i, -1)[kDirRight] = 0;
//        }
//    }
//    
//    void HashIn(char row, char col) {
//        HashIn({row, col});
//    }
//    
//    void HashIn(const Position& p) {
//        hash_function_->xorNothing(&hash_); // xor out 
//        hash_function_->xorState(&hash_, p, 0);
//    }
//    
//    void HashOut(const Position& p) {
//        hash_function_->xorState(&hash_, p, 0); // xor out
//        hash_function_->xorNothing(&hash_); // xor in
//    }
//    
//    // when doing cast i can already go with Direction and index
//    
//    vector<tuple<short, Direction>> borders_;
//    
//    // expect (0, 0), (board_size_+2, board_size_+2)
//    Position ToPosition(short indeex) {
//        return {index/(board_size_+2)-1, index%(board_size_+2)-1};
//    }
//    
//    // expect from (-1,-1) to (board_size_, board_size_)
//    short ToIndex(const Position& p) {
//        return (p.row+1)*(board_size_ + 2) + (p.col+1)
//    }
//    
//    Ray NextFromMirror(Ray ray, Mirror mir) {
//        if (ray.dir)...
//        
//        Direction dir = kDirReflection[mir][ray.dir];
//        return {neighbors_[ray.pos][dir], dir};
//    }
//    
//    // same as for FromBorder
//    Ray NextFromEmpty(Ray ray) {
//        return {neighbors_[ray.pos][ray.dir], ray.dir};
//    }
//
//public:
//    
//    void Cast(Ray ray) {
//        auto& mirs = *mirrors_;
//        
//        history_casts_.push_back(ray.pos);
//        ray = NextFromEmpty(ray);
//        Count count = 0;
//        while (mirs[ray.pos] != kMirBorder) {
//            Destroy({0, 0});
//            DestroyLinks(ray.pos);
//            ray = NextFromMirror(ray, mirs[ray.pos]);
//            ++count;
//        }    
//        destroyed_count_ += count;
//    }
//    
//    void DestroyLinks(short pos) {
//        auto& n = neighbors_[pos];
//        neighbors_[n[kDirTop]][kDirBottom] = n[kDirBottom];
//        neighbors_[n[kDirBottom]][kDirTop] = n[kDirTop];
//        neighbors_[n[kDirLeft]][kDirRight] = n[kDirRight];
//        neighbors_[n[kDirRight]][kDirLeft] = n[kDirLeft];
//    }
//
//    void CastRestorable(Ray ray) {
//        auto& last = *last_cast_; 
//        auto& mirs = *mirrors_;
//        
//        ray = NextFromEmpty(ray);
//        while (mirs[ray.pos] != kMirBorder) {
//            if (mirs[ray.pos] >= kMirOffset) {
//                ray = NextFromEmpty(ray);
//                continue;
//            } 
//            last.push_back(ray.pos);
//            Destroy({0, 0});
//            mirs[ray.pos] += kMirOffset;
//            ray = NextFromMirror(ray, mirs[ray.pos]);
//        }    
//        destroyed_count_ += last.size();
//    }
//    
//    void Restore() {
//        auto& last = *last_cast_; 
//        auto& mirs = *mirrors_;
//        
//        destroyed_count_ -= last.size();
//        while (!last.empty()) {
//            mirs[last.back()] -= kMirOffset;
//            Restore({0, 0});
//            last.pop_back();
//        }
//    }
//    
//    void Destroy(const Position& p) {
//        if (--mirrors_left_[kOrientHor][p.col] == 0) {
//            ++empty_lines_count_;
//            // empty is not counted as even?????
//        } else if (mirrors_left_[kOrientHor][p.col] % 2 == 0) {
//            ++even_mirrors_lines_;
//        } else {
//            --even_mirrors_lines_;
//        }
//        
//        if (--mirrors_left_[kOrientVer][p.row] == 0) {
//            ++empty_lines_count_;
//        } else if (mirrors_left_[kOrientVer][p.row] % 2 == 0) {
//            ++even_mirrors_lines_;
//        } else {
//            --even_mirrors_lines_;
//        }
//        HashOut(p);
//    }
//     
//    void Restore(const Position& p) {
//        if (++mirrors_left_[kOrientHor][p.col] == 1) {
//            --empty_lines_count_;
//        }
//        if (mirrors_left_[kOrientHor][p.col] % 2 == 0) {
//            ++even_mirrors_lines_;
//        } else {
//            --even_mirrors_lines_;
//        }
//        
//        if (++mirrors_left_[kOrientVer][p.row] == 1) {
//            --empty_lines_count_;
//        }
//        if (mirrors_left_[kOrientVer][p.row] % 2 == 0) {
//            ++even_mirrors_lines_;
//        } else {
//            --even_mirrors_lines_;
//        }
//        HashIn(p);
//    }
//    
//    Count CastCount() const {
//        return history_casts_.size();
//    }
//      
//    Int board_size() const {
//        return board_size_;
//    } 
//    
//    Int size() const {
//        return board_size_;
//    }
//    
//    const HashType& hash() const {
//        return hash_;
//    }
//    
//    bool AllDestroyed() const {
//        return destroyed_count_ == board_size_*board_size_;
//    }
//    
//    const vector<Position>& HistoryCasts() const {
//        return {};
//    }
//    
//    Count MirrorsDestroyed() const {
//        return destroyed_count_;
//    }
//    
//    Count EvenMirrorsLines() {
//        return even_mirrors_lines_;
//    }
//    
//    Count EmptyLinesCount() const {
//        return empty_lines_count_;   
//    }
//};
//
//

#endif
