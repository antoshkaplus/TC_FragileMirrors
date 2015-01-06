//
//  board2.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/15/14.
//
//  Adding full sized history to retrieve number of lines on
//  each step for the winning algorithm.
//  should keep it inside another Position, probably template char
//
// usage of shared_ptr<CastNode>


#ifndef FRAGILE_MIRRORS_board2_hpp
#define FRAGILE_MIRRORS_board2_hpp

#include "board_v1.hpp"

#include <iostream>
#include <vector>
#include <stack>
#include <array>
#include <type_traits>
#include <tuple>

#include <ant/core.h>
#include <ant/grid.h>

#include "util.hpp"
#include "board.hpp"

using namespace std;
using namespace ant;
using namespace ant::grid;


class Board_v2 {
public:
    struct CastNode {
        Position cast;
        shared_ptr<CastNode> previous;
    };
    
private:
    using Direction = char;
    
    const constexpr static int kDirTop      = 0;
    const constexpr static int kDirBottom   = 1;
    const constexpr static int kDirLeft     = 2;
    const constexpr static int kDirRight    = 3;
    const constexpr static int kDirNothing  = 4;
    
    const constexpr static char kMirRight     = 0;
    const constexpr static char kMirLeft      = 1;
    const constexpr static char kMirBorder    = 2;
    
    const constexpr static char kOrientHor = 0;
    const constexpr static char kOrientVer = 1;
    
    constexpr const static array<int, 5> kDirOpposite = { {
        kDirBottom, 
        kDirTop, 
        kDirRight, 
        kDirLeft, 
        kDirNothing
    } };
    
    Direction FromDirection(const Board_v2& b, const Position& p) {
        if (p.row == -1) {
            return kDirTop;
        } else if (p.col == -1) {
            return kDirLeft;
        } else if (p.row == b.board_size()) {
            return kDirBottom;
        } else if (p.col == b.board_size()){
            return kDirRight;
        } else {
            throw runtime_error("cant deduct direction from position");
        }
    }
    
    constexpr static size_t HashBitsCount = 64;
    // supports -1, -1 origin now
    using Neighbors = OriginGrid<Grid<array<int8_t, 4>>>;
    using Mirrors = OriginGrid<Grid<int8_t>>;
    
public:    
    using HashFunction = ZobristHashing<HashBitsCount>;
    using HashType = typename HashFunction::value;
    
    Board_v2() {}
    
    Board_v2(const vector<string>& board) : destroyed_count_(0) {
        board_size_ = board.size();
        Count expanded_size = board_size_ + 2;
        
        Position origin = {-1, -1};
        Size size = {expanded_size, expanded_size};
        // seeting up mirrors grid
        hash_function_.reset(new HashFunction({board_size_, board_size_}, 1));
        // setting up neighbors
        mirrors_.reset(new Mirrors(origin, size));
        auto& mirs = *mirrors_;
        for (auto row = 0; row < board_size_; ++row) {
            for (auto col = 0; col < board_size_; ++col) {
                mirs(row, col) = IsRightMirror(board[row][col]) ? kMirRight : kMirLeft;
            }
        }  
        // need some adjustments for sides
        for (auto i = 0; i < board_size_; ++i) {
            mirs(-1, i) = mirs(board_size_, i) 
            = mirs(i, -1) = mirs(i, board_size_) = kMirBorder;
        }
        
        InitNeighbors();
    }
    
private:
    
    void InitNeighbors() {
        neighbors_.origin() = {-1, -1};
        neighbors_.grid().resize({board_size_+2, board_size_+2});
        for (auto row = 0; row < board_size_; ++row) {
            for (auto col = 0; col < board_size_; ++col) {
                auto& ns = neighbors_(row, col);
                ns[kDirTop]     = row-1;
                ns[kDirBottom]  = row+1;
                ns[kDirLeft]    = col-1;
                ns[kDirRight]   = col+1;
                HashIn(row, col);
            }
        }  
        for (auto i = 0; i < board_size_; ++i) {
            neighbors_(-1, i)[kDirBottom] = 0;
            neighbors_(board_size_, i)[kDirTop] = board_size_-1;
            neighbors_(i, board_size_)[kDirLeft] = board_size_-1;
            neighbors_(i, -1)[kDirRight] = 0;
        }
    }
    
    void HashIn(char row, char col) {
        HashIn({row, col});
    }
    
    void HashIn(const Position& p) {
        hash_function_->xorNothing(&hash_); // xor out 
        hash_function_->xorState(&hash_, p, 0);
    }
    
    void HashOut(const Position& p) {
        hash_function_->xorState(&hash_, p, 0); // xor out
        hash_function_->xorNothing(&hash_); // xor in
    }
    
public:
    
    bool operator==(const Board_v1& b) const {
        return neighbors_.grid() == b.neighbors_.grid();
    }
    
    Board_v2 AfterCasts(Count cast_count) const {
        Board_v2 b;
        b.board_size_ = board_size_;
        b.destroyed_count_ = 0;
        b.hash_function_ = hash_function_;
        b.mirrors_ = mirrors_;
        b.InitNeighbors();
        
        vector<Position> casts;
        shared_ptr<CastNode> node = history_casts_;
        while (node) {
            casts.push_back(node->cast);
            node = node->previous;
        }
        reverse(casts.begin(), casts.end());
        for (int i = 0; i < cast_count; ++i) {
            b.Cast(casts[i]);
        }
        return b;
    }
    
    Board_v2 InitWithCasts(shared_ptr<CastNode> sequence) const {
        Board_v2 b;
        b.board_size_ = board_size_;
        b.destroyed_count_ = 0;
        b.hash_function_ = hash_function_;
        b.mirrors_ = mirrors_;
        b.InitNeighbors();
        
        vector<Position> casts;
        shared_ptr<CastNode> node = sequence;
        while (node) {
            casts.push_back(node->cast);
            node = node->previous;
        }
        reverse(casts.begin(), casts.end());
        for (int i = 0; i < casts.size(); ++i) {
            b.Cast(casts[i]);
        }
        return b;
    }
    
    
    
//    Count CastCount() const {
//        return history_casts_.size();
//    }
    
    // better return count of destroyed
    // to know if anything was destroyed at all
    Count Cast(const Position& ppp) {
        ++cast_count_;
        last_cast_.clear();
        
        CastNode* new_cast = new CastNode();
        new_cast->cast = ppp;
        new_cast->previous = history_casts_;
        history_casts_.reset(new_cast); 
        
        Position p = ppp;
        auto& mirs = *mirrors_;
        Direction dir = FromDirection(*this, p);
        tie(p, dir) = NextFrom(p, dir);
        Count count = 0;
        while (mirs[p] != kMirBorder) {
            Destroy(p);
            last_cast_.push_back(p);
            tie(p, dir) = NextFrom(p, dir);
            ++count;
        }    
        destroyed_count_ += count;
        return count;
    }
    
    void Restore() {
        --cast_count_;
        history_casts_ = history_casts_->previous;
        destroyed_count_ -= last_cast_.size();
        while (!last_cast_.empty()) {
            Restore(last_cast_.back());
            last_cast_.pop_back();
        }
    }
    
    Int board_size() const {
        return board_size_;
    } 
    
    Int size() const {
        return board_size_;
    }
    
    const HashType& hash() const {
        return hash_;
    }
    
    bool AllDestroyed() const {
        return destroyed_count_ == board_size_*board_size_;
    }
    
    const shared_ptr<CastNode>& HistoryCasts() const {
        return history_casts_;
    }
    
    Count MirrorsDestroyed() const {
        return destroyed_count_;
    }
    
    Count EmptyLinesCount() const {
        Count count = 0;
        for (int i = 0; i < board_size_; ++i) {
            if (neighbors_(-1, i)[kDirBottom] == board_size_) ++count;
            if (neighbors_(i, -1)[kDirRight] == board_size_) ++count;
        }
        return count;
    }
    
    Count LastCastCount() const {
        return last_cast_.size();
    }
    
    const vector<Position>& last_cast() const {
        return last_cast_;
    }
    
    bool IsLastIsolated() const {
        if (last_cast_.size() == 1) {
            return IsEmptyCross(last_cast_[0]);
        } else if (last_cast_.size() == 2) {
            return IsEmptyCross(last_cast_[0]) 
            && IsEmptyCross(last_cast_[1]);
        }
        return false; 
    }
    
    bool IsEmptyCross(const Position& p) const {
        return neighbors_(-1, p.col)[kDirBottom] == board_size_ 
        && neighbors_(p.row, -1)[kDirRight] == board_size_;
    }
    
    Count CastCount() const {
        return cast_count_;
    }
    
private:
    
    // returns next position and from which direction ray will come to that position
    tuple<Position, Direction> NextFrom(const Position& p, Direction dir) const {
        using P = tuple<Position, char>;
        auto& mirs = *mirrors_;
        auto& ns = neighbors_[p]; 
        switch (mirs[p]) {
            case kMirRight: {
                switch (dir) {
                    case kDirTop:       return P{{p.row, ns[kDirRight]}, kDirLeft} ;    
                    case kDirBottom:    return P{{p.row, ns[kDirLeft]}, kDirRight};     
                    case kDirLeft:      return P{{ns[kDirBottom], p.col}, kDirTop};   
                    case kDirRight:     return P{{ns[kDirTop], p.col}, kDirBottom};  
                    default: assert(false);    
                }
            }
            case kMirLeft: {
                switch (dir) {
                    case kDirTop:       return P{{p.row, ns[kDirLeft]}, kDirRight} ;    
                    case kDirBottom:    return P{{p.row, ns[kDirRight]}, kDirLeft};     
                    case kDirLeft:      return P{{ns[kDirTop], p.col}, kDirBottom};   
                    case kDirRight:     return P{{ns[kDirBottom], p.col}, kDirTop};  
                    default: assert(false); 
                }
            }
            case kMirBorder: {
                int d = -1;
                switch (dir) {
                    case kDirRight:
                    case kDirLeft:
                        return P{{p.row, ns[kDirOpposite[d = dir]]}, dir};
                        break;
                    case kDirTop:
                    case kDirBottom:
                        return P{{ns[kDirOpposite[d = dir]], p.col}, dir};
                        break;
                    default: assert(false);
                }
            }
            default: {
                cout << mirs[p];
                assert(false);
            }
        }
        throw runtime_error("");
    }
    
    
    void Restore(const Position& p) {
        auto& n = neighbors_(p);
        neighbors_(n[kDirTop], p.col)[kDirBottom] = p.row;
        neighbors_(n[kDirBottom], p.col)[kDirTop] = p.row;
        neighbors_(p.row, n[kDirLeft])[kDirRight] = p.col;
        neighbors_(p.row, n[kDirRight])[kDirLeft] = p.col;
        HashIn(p);
    }
    
    void Destroy(const Position& p) {
        auto& n = neighbors_(p);
        neighbors_(n[kDirTop], p.col)[kDirBottom] = n[kDirBottom];
        neighbors_(n[kDirBottom], p.col)[kDirTop] = n[kDirTop];
        neighbors_(p.row, n[kDirLeft])[kDirRight] = n[kDirRight];
        neighbors_(p.row, n[kDirRight])[kDirLeft] = n[kDirLeft];
        HashOut(p);
    }
    
    // should initialize only once in constructor probably
    Neighbors neighbors_;
    // sum of history_count_
    Count destroyed_count_;
    HashType hash_;
    Int board_size_;
    Count cast_count_{0};
    
    vector<Position> last_cast_;
    shared_ptr<CastNode> history_casts_;
    
    // those guys are initialized when initialization done
    // through vector of strings
    shared_ptr<Mirrors> mirrors_;
    // assume board_size without borders
    shared_ptr<HashFunction> hash_function_;
    
    friend class Board_v2;
};


#endif
