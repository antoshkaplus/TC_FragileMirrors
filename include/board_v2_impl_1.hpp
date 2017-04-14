//
//  board_v4.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 1/4/15.
//
//

#ifndef FRAGILE_MIRRORS_board_v4_hpp
#define FRAGILE_MIRRORS_board_v4_hpp

#include "board_common.hpp"

using namespace std;

// space optimization
// ray indexes are used
template <class CastHistoryType>
class Board_v2_Impl_1 : public Board_v2_Reduce {
private:
    
    using int8_t = short;
    
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
    using Neighbors = std::array<short, 4>;

private:
    // divide this dude on multiple dudes
    // Neighbors
    // row, col only for ray.
    // new hash function with border as changing state based of number of rays from particular place 
    // mirror = separate dude char array
    // destroyed = can be known different way
    // 

    // probably can place deleted and 

    struct  Item  {
        Neighbors neighbors;
        int8_t row;
        int8_t col;
        char mirror;
        bool destroyed;
    };

    struct Ray {
        Ray(short pos, Direction dir) 
        : pos(pos), dir(dir) {}
        
        short pos;
        Direction dir; 
    };    
    
    constexpr static array<int, 5> kDirOpposite= { {
           kDirBottom,
           kDirTop,
           kDirRight,
           kDirLeft,
           kDirNothing
   } };

    // first index mirror type
    // second index where ray going
    // result direction where will go  
    constexpr static array<array<char, 4>, 2> kDirReflection =  { {
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
    
    Board_v2_Impl_1() {}
    
    Board_v2_Impl_1(const vector<string>& str_board) : board_size_(str_board.size()),
                                                       board_hash_(board_size_) {
        filled_space_ = str_board.size()*str_board.size();
        empty_space_ = 0;

        mirrors_destroyed_ = 0;
        even_mirrors_lines_ = 2 * board_size_ * (board_size_ % 2 == 0 ? 1 : 0);
        empty_lines_count_ = 0;
        
        mirrors_left_[kOrientHor].resize(board_size_, board_size_);
        mirrors_left_[kOrientVer].resize(board_size_, board_size_);
        
        items_.resize(4*board_size_ + board_size_*board_size_);
        ray_direction_.resize(4*board_size_);
        
        restorable_buffer_.reset(new vector<short>());
        reduce_buffer_.reset(new vector<short>());
        
        // initializing inner links
        auto offset = 4*board_size_;
        auto ToIndex = [&](int r, int c) {
            return r * board_size_ + c + offset;
        };
        for (int r = 0; r < board_size_; ++r) {
            for (int c = 0; c < board_size_; ++c) {
                Index i = ToIndex(r, c);
                auto& t = items_[i]; 
                auto& ns = t.neighbors;
                ns[kDirTop] = ToIndex(r-1, c);
                ns[kDirRight] = ToIndex(r, c+1);
                ns[kDirBottom] = ToIndex(r+1, c);
                ns[kDirLeft] = ToIndex(r, c-1);
                t.mirror = IsRightMirror(str_board[r][c]) ? kMirRight : kMirLeft;
                t.row = r;
                t.col = c;
                board_hash_.HashIn(r, c);
            }
        }
        // initializing border links
        for (int i = 0; i < board_size_; ++i) {
            int s = 4*i;
            int mir_index;
            int border_index;
            Item *t;
            
            // TOP
            mir_index = ToIndex(0, i);
            border_index = s + kDirTop;
            t = &items_[border_index];
            t->neighbors.fill(-1);
            t->neighbors[kDirBottom] = mir_index;
            t->row = -1;
            t->col = i;
            ray_direction_[border_index] = kDirBottom;
            items_[mir_index].neighbors[kDirTop] = border_index;
            
            
            // RIGHT
            mir_index = ToIndex(i, board_size_-1);
            border_index = s + kDirRight;
            t = &items_[border_index];
            t->neighbors.fill(-1);
            t->neighbors[kDirLeft] = mir_index;
            t->row = i;
            t->col = board_size_;
            ray_direction_[border_index] = kDirLeft;
            items_[mir_index].neighbors[kDirRight] = border_index;
            
            // BOTTOM
            mir_index = ToIndex(board_size_-1, i);
            border_index = s + kDirBottom;
            t = &items_[border_index];
            t->neighbors.fill(-1);
            t->neighbors[kDirTop] = mir_index;
            t->row = board_size_;
            t->col = i;
            ray_direction_[border_index] = kDirTop;
            items_[mir_index].neighbors[kDirBottom] = border_index;
            
            // LEFT
            mir_index = ToIndex(i, 0);
            border_index = s + kDirLeft; 
            t = &items_[border_index];
            t->neighbors.fill(-1);
            t->neighbors[kDirRight] = mir_index;
            t->row = i;
            t->col = -1;
            ray_direction_[border_index] = kDirRight;
            items_[mir_index].neighbors[kDirLeft] = border_index;
        }
        for (int i = 0; i < items_.size(); ++i) {
            items_[i].destroyed = false;
        } 
    }
    
    
    Count CastRestorable(short ray_index) override {
        auto& last = *restorable_buffer_; 
        
        Ray ray{ray_index, ray_direction_[ray_index]};
        ray = NextFromEmpty(ray);
        while (ray.pos >= ray_direction_.size()) {
            auto& t = items_[ray.pos]; 
            if (t.destroyed) {
                ray = NextFromEmpty(ray);
                continue;
            } 
            last.push_back(ray.pos);
            Destroy(t.row, t.col);
            t.destroyed = true;
            ray = NextFromMirror(ray);
        }    
        mirrors_destroyed_ += last.size();
        return last.size();
    }
    
    Count CastImpl(short ray_index) override {
        auto& ray_item = items_[ray_index];
        history_casts_.Push({ray_item.row, ray_item.col});

        Ray ray = NextFromBorder(ray_index);
        Count count = 0;
        while (ray.pos >= ray_direction_.size()) {
            items_[ray.pos].destroyed = true;
            Destroy(items_[ray.pos].row, items_[ray.pos].col);
            DestroyLinks(ray.pos);
            ray = NextFromMirror(ray);
            ++count;
        }    
        empty_space_ += count;
        filled_space_ -= count;
        mirrors_destroyed_ += count;
        return count;
    }

    Count CastCount() const override {
        return history_casts_.Count();
    }

    void Restore() override {
        auto& last = *restorable_buffer_; 
        
        mirrors_destroyed_ -= last.size();
        while (!last.empty()) {
            auto& t = items_[last.back()]; 
            t.destroyed = false;
            Restore(t.row, t.col);
            last.pop_back();
        }
    }
    
    void Destroy(char row, char col) {
        if (--mirrors_left_[kOrientHor][col] == 0) {
            ++empty_lines_count_;
            // empty is not counted as even?????
        } /*else if (mirrors_left_[kOrientHor][col] % 2 == 0) {
            ++even_mirrors_lines_;
        } else {
            --even_mirrors_lines_;
        }*/
        
        if (--mirrors_left_[kOrientVer][row] == 0) {
            ++empty_lines_count_;
        } /*else if (mirrors_left_[kOrientVer][row] % 2 == 0) {
            ++even_mirrors_lines_;
        } else {
            --even_mirrors_lines_;
        }*/
        board_hash_.HashOut({row, col});
    }
    
    void DestroyLinks(short index) {
        auto& ns = items_[index].neighbors;
        items_[ns[kDirTop]].neighbors[kDirBottom] = ns[kDirBottom];
        items_[ns[kDirBottom]].neighbors[kDirTop] = ns[kDirTop];
        items_[ns[kDirLeft]].neighbors[kDirRight] = ns[kDirRight];
        items_[ns[kDirRight]].neighbors[kDirLeft] = ns[kDirLeft];
    }
    
    void Restore(char row, char col) {
        if (++mirrors_left_[kOrientHor][col] == 1) {
            --empty_lines_count_;
        } /*else if (mirrors_left_[kOrientHor][col] % 2 == 0) {
            ++even_mirrors_lines_;
        } else {
            --even_mirrors_lines_;
        }*/
        
        if (++mirrors_left_[kOrientVer][row] == 1) {
            --empty_lines_count_;
        } /*else if (mirrors_left_[kOrientVer][row] % 2 == 0) {
            ++even_mirrors_lines_;
        } else {
            --even_mirrors_lines_;
        }*/
        board_hash_.HashIn({row, col});
    }
    
    bool IsEmptyLine(short ray_index) {
        return NextFromBorder(ray_index).pos < RayCount();
        
    }
    
    void Reduce(vector<short>& shift) {
        Reduce();
        auto& offset = *reduce_buffer_;
        for (auto& s : shift) {
            s -= offset[s];
        }
    }
    
    // 4 * Number of items
    void Reduce() override {
        auto& offset = *reduce_buffer_;
        offset.resize(items_.size());
        for (auto i = 0; i < ray_direction_.size(); ++i) {
            if (IsEmptyLine(i)) {
                items_[i].destroyed = true;
            } 
        }
        auto cur = 0;
        for (auto i = 0; i < items_.size(); ++i) {
            if (items_[i].destroyed) {
                ++cur;
            } 
            offset[i] = cur;
        }
        
        for (auto i = 0; i < ray_direction_.size(); ++i) {
            if (items_[i].destroyed) {
                continue;
            }
            short& p = items_[i].neighbors[ray_direction_[i]];
            p -= offset[p];
            items_[i - offset[i]] = items_[i];
            ray_direction_[i - offset[i]] = ray_direction_[i];
        }
        for (auto i = ray_direction_.size(); i < items_.size(); ++i) {
            if (items_[i].destroyed) {
                continue;
            }
            auto& t = items_[i].neighbors; 
            for (int q = 0; q < 4; ++q) {
                t[q] -= offset[t[q]];
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

    Count EvenLinesCount() const {
        return even_mirrors_lines_;
    }

    BoardHash::HashType hash() const override {
        return board_hash_.hash();
    }
    
    Count EmptySpace() const override {
        return empty_space_;
    }
    
    Count FilledSpace() const {
        return filled_space_;
    }

    Count TotalSpace() const override {
        return items_.size();
    }

    vector<Position> CastHistory() const override {
        return ToVector(history_casts_);
    }

    unique_ptr<Board> Clone() const override {
        return move(make_unique<Board_v2_Impl_1>(*this));
    }


private:

    Ray NextFromMirror(const Ray& ray) const {
        Direction dir = kDirReflection[items_[ray.pos].mirror][ray.dir];
        return {items_[ray.pos].neighbors[dir], dir};
    }
    
    Ray NextFromBorder(short ray_index) const {
        Direction dir = ray_direction_[ray_index];
        return {items_[ray_index].neighbors[dir], dir};
    }
    
    // same as for FromBorder
    Ray NextFromEmpty(const Ray& ray) const {
        return {items_[ray.pos].neighbors[ray.dir], ray.dir};
    }
    
    void VerifyAfterReduce() {
        vector<bool> used(items_.size(), false);
        for (int i = 0; i < ray_direction_.size(); ++i) {
            int count = 0;
            char dir = 0;
            for (auto n : items_[i].neighbors) {
                if (n == -1) ++count; 
                else {
                    assert(ray_direction_[i] == dir);
                    //assert(!used[n]);
                    used[n] = true;
                }
                ++dir;
            }
            assert(count == 3);
        }
        for (int i = ray_direction_.size(); i < items_.size(); ++i) {
            for (auto n : items_[i].neighbors) {
                assert(n != -1);
                //assert(!used[n]);
                used[n] = true;
            }
        }
    }

    vector<Item> items_;
    // they are first in items
    // where is ray directed
    vector<Direction> ray_direction_;

    array<vector<char>, 2> mirrors_left_;
    Count empty_lines_count_;
    Count even_mirrors_lines_;

    Count board_size_;
    // can make use of cast node actually
    // vector<>

    Count filled_space_;
    Count empty_space_;

    Count mirrors_destroyed_;

    CastHistoryType history_casts_;

    BoardHash board_hash_;

    // hash function

    shared_ptr<vector<short>> restorable_buffer_;
    shared_ptr<vector<short>> reduce_buffer_;

};

template<class T>
constexpr array<int, 5> Board_v2_Impl_1<T>::kDirOpposite;
template<class T>
constexpr array<array<char, 4>, 2> Board_v2_Impl_1<T>::kDirReflection;

#endif
