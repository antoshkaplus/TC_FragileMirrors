//
//  board_v1.h
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/26/14.
//
//

#ifndef FRAGILE_MIRRORS_board_v1_h
#define FRAGILE_MIRRORS_board_v1_h

#include "board_common.hpp"

using namespace std;
using namespace ant;
using namespace ant::grid;


template<class CastHistoryType>
class Board_v1_Impl_1 : public Board_v1 {

    // supports -1, -1 origin now
    using Neighbors = OriginGrid<Grid<array<int8_t, 4>>>;
    using Mirrors = OriginGrid<Grid<int8_t>>;
public:
    using CastType = Position;
    using HashType = BoardHash::HashType;


    Board_v1_Impl_1() {}

    Board_v1_Impl_1(const vector<string>& board) : destroyed_count_(0),
                                            board_size_((int)board.size()),
                                            board_hash_(board_size_) {
        InitCastCandidates();
        InitMirrors(board);
        InitNeighbors();
    }

    bool operator==(const Board_v1_Impl_1& b) const {
        return neighbors_.grid() == b.neighbors_.grid();
    }

    // TODO
    Board_v1_Impl_1 AfterCasts(Count cast_count) const {
        Board_v1_Impl_1 b;
        b.board_size_ = board_size_;
        b.destroyed_count_ = 0;
        b.board_hash_ = board_hash_;
        b.board_hash_.clear();
        b.mirrors_ = mirrors_;
        b.InitNeighbors();

        history_casts_.ForEachUntil(cast_count, [&](const Position& p) {
            b.Cast(p);
        });
        return b;
    }

    Count CastCount() const override {
        return history_casts_.Count();
    }

    // return count of destroyed
    // to know if anything was destroyed at all
    Count Cast(const Position& ppp) override {
        last_cast_.clear();
        history_casts_.Push(ppp);
        Position p = ppp;
        auto& mirs = *mirrors_;
        Direction dir = FromDirection(p);
        tie(p, dir) = NextFrom(p, dir);
        Count count = 0;
        while (mirs[p] != kMirBorder) {
            Destroy(p);
            last_cast_.push_back(p);
            tie(p, dir) = NextFrom(p, dir);
            ++count;
        }
        destroyed_count_ += count;
        assert(destroyed_count_ <= mirror_count());
        return count;
    }

    const vector<Position>& CastCandidates() const override {
        return *cast_candidates_;
    }

    void Restore() override {
        history_casts_.Pop();
        destroyed_count_ -= last_cast_.size();
        assert(destroyed_count_ >= 0);
        while (!last_cast_.empty()) {
            Restore(last_cast_.back());
            last_cast_.pop_back();
        }
    }

    Int board_size() const {
        return board_size_;
    }

    Int size() const override {
        return board_size_;
    }

    HashType hash() const override {
        return board_hash_.hash();
    }

    bool AllDestroyed() const override {
        assert(destroyed_count_ <= mirror_count());
        return destroyed_count_ == mirror_count();
    }

    // maybe think about something different
    vector<Position> CastHistory() const override {
        return ToVector(history_casts_);
    }

    Count MirrorsDestroyed() const override {
        return destroyed_count_;
    }

    Count mirror_count() const {
        return size()*size();
    }

    Count EmptyLinesCount() const override {
        Count count = 0;
        for (int i = 0; i < board_size_; ++i) {
            if (neighbors_(-1, i)[kDirDown] == board_size_) ++count;
            if (neighbors_(i, -1)[kDirRight] == board_size_) ++count;
        }
        return count;
    }

    bool IsLineEmpty(Position p) const override {
        Direction dir = FromDirection(p);
        tie(p, dir) = NextFrom(p, dir);
        auto& mirs = *mirrors_;
        return mirs[p] == kMirBorder;
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
        return neighbors_(-1, p.col)[kDirDown] == board_size_
               && neighbors_(p.row, -1)[kDirRight] == board_size_;
    }

    const auto& mirrors() const {
        return *mirrors_;
    }

    bool IsCastPosition(const Position& p) const {
        return p.row == -1 || p.col == -1 || p.row == board_size() || p.col == board_size();
    }


    unique_ptr<Board> Clone() const override {
        return move(make_unique<Board_v1_Impl_1>(*this));
    }

    ~Board_v1_Impl_1() {}

private:
    
    Direction FromDirection(const Position& p) const {
        if (p.row == -1) {
            return kDirUp;
        } else if (p.col == -1) {
            return kDirLeft;
        } else if (p.row == board_size()) {
            return kDirDown;
        } else if (p.col == board_size()){
            return kDirRight;
        } else {
            stringstream s;
            s << "cant deduct direction from position ";
            s << p;
            throw runtime_error(s.str());
        }
    }

    void InitCastCandidates() {
        cast_candidates_.reset(new vector<Position>());
        cast_candidates_->reserve(4*board_size_);
        auto& c = *cast_candidates_;
        for (Index i = 0; i < board_size_; ++i) {
            c.emplace_back(i, -1);
            c.emplace_back(i, board_size_);
            c.emplace_back(-1, i);
            c.emplace_back(board_size_, i);
        }
    }

    void InitMirrors(const vector<string>& board) {
        Count expanded_size = board_size_ + 2;
        Position origin = {-1, -1};
        grid::Size size = {expanded_size, expanded_size};

        mirrors_.reset(new Mirrors(origin, size));
        auto& mirs = *mirrors_;
        auto func = [&](const Position& p) {
            mirs(p) = IsRightMirror(board[p.row][p.col]) ? kMirRight : kMirLeft;
        };
        Region(Position{0, 0}, Size(board_size_, board_size_)).ForEach(func);
        // need some adjustments for sides
        for (auto i = 0; i < board_size_; ++i) {
            mirs(-1, i) = mirs(board_size_, i)
                    = mirs(i, -1) = mirs(i, board_size_) = kMirBorder;
        }
    }

    void InitNeighbors() {
        neighbors_.origin() = {-1, -1};
        neighbors_.grid().resize({board_size_+2, board_size_+2});
        for (auto row = 0; row < board_size_; ++row) {
            for (auto col = 0; col < board_size_; ++col) {
                auto& ns = neighbors_(row, col);
                ns[kDirUp]     = row-1;
                ns[kDirDown]  = row+1;
                ns[kDirLeft]    = col-1;
                ns[kDirRight]   = col+1;
                board_hash_.HashIn(row, col);
            }
        }  
        for (auto i = 0; i < board_size_; ++i) {
            neighbors_(-1, i)[kDirDown] = 0;
            neighbors_(board_size_, i)[kDirUp] = board_size_-1;
            neighbors_(i, board_size_)[kDirLeft] = board_size_-1;
            neighbors_(i, -1)[kDirRight] = 0;
        }
    }

    // returns next position and from which direction ray will come to that position
    tuple<Position, Direction> NextFrom(const Position& p, Direction dir) const {
        using P = tuple<Position, char>;
        auto& mirs = *mirrors_;
        auto& ns = neighbors_[p]; 
        switch (mirs[p]) {
            case kMirRight: {
                switch (dir) {
                    case kDirUp:       return P{{p.row, ns[kDirRight]}, kDirLeft} ;    
                    case kDirDown:    return P{{p.row, ns[kDirLeft]}, kDirRight};     
                    case kDirLeft:      return P{{ns[kDirDown], p.col}, kDirUp};   
                    case kDirRight:     return P{{ns[kDirUp], p.col}, kDirDown};  
                    default: assert(false);    
                }
            }
            case kMirLeft: {
                switch (dir) {
                    case kDirUp:       return P{{p.row, ns[kDirLeft]}, kDirRight} ;    
                    case kDirDown:    return P{{p.row, ns[kDirRight]}, kDirLeft};     
                    case kDirLeft:      return P{{ns[kDirUp], p.col}, kDirDown};   
                    case kDirRight:     return P{{ns[kDirDown], p.col}, kDirUp};  
                    default: assert(false); 
                }
            }
            case kMirBorder: {
                int d = -1;
                switch (dir) {
                    case kDirRight:
                    case kDirLeft:
                        return P{{p.row, ns[kDirOpposite[d = dir]]}, (char)dir};
                        break;
                    case kDirUp:
                    case kDirDown:
                        return P{{ns[kDirOpposite[d = dir]], p.col}, (char)dir};
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
        neighbors_(n[kDirUp], p.col)[kDirDown] = p.row;
        neighbors_(n[kDirDown], p.col)[kDirUp] = p.row;
        neighbors_(p.row, n[kDirLeft])[kDirRight] = p.col;
        neighbors_(p.row, n[kDirRight])[kDirLeft] = p.col;
        board_hash_.HashIn(p);
    }
     
    void Destroy(const Position& p) {
        auto& n = neighbors_(p);
        neighbors_(n[kDirUp], p.col)[kDirDown] = n[kDirDown];
        neighbors_(n[kDirDown], p.col)[kDirUp] = n[kDirUp];
        neighbors_(p.row, n[kDirLeft])[kDirRight] = n[kDirRight];
        neighbors_(p.row, n[kDirRight])[kDirLeft] = n[kDirLeft];
        board_hash_.HashOut(p);
    }

    // should initialize only once in constructor probably
    Neighbors neighbors_;
    // sum of history_count_
    Count destroyed_count_;
    Int board_size_;

    BoardHash board_hash_;

    vector<Position> last_cast_;
    CastHistoryType history_casts_;

    shared_ptr<Mirrors> mirrors_;

    shared_ptr<vector<Position>> cast_candidates_;



    friend class Board_v2;
};



#endif
