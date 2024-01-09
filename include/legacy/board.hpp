#pragma once
#include <ant/core/core.hpp>
#include "util.hpp"


class Board {
public:
    using HashType = bitset<64>;

    virtual HashType hash() const = 0;
    virtual Count MirrorsLeft() const {
        return (Count)(size()* size() - MirrorsDestroyed());
    }
    virtual Count MirrorsDestroyed() const = 0;
    virtual Count size() const = 0;

    virtual Count EmptyLinesCount() const = 0;

    virtual Count CastCount() const = 0;

    virtual vector<Position> CastHistory() const = 0;

    virtual bool AllDestroyed() const {
        return MirrorsLeft() == 0;
    }

    virtual unique_ptr<Board> Clone() const = 0;

    virtual ~Board() = default;
};


// Restore works only once after cast
class Board_v1 : public Board {
public:
    using CastType = Position;

    virtual bool IsLineEmpty(Position p) const = 0;
    virtual Count Cast(const Position& p) = 0;
    virtual void Restore() = 0;
    virtual const vector<Position>& CastCandidates() const = 0;

    template<class Functor>
    void ForEachAppliedCast(Functor func) {
        for (auto& p : CastCandidates()) {
            Cast(p);
            func(p);
            Restore();
        }
    }

    virtual ~Board_v1() = default;
};


class Board_v2 : public Board {
public:
    using CastType = short ;

    virtual Count Cast(short ray_index) = 0;
    virtual void Restore() = 0;
    virtual Count CastRestorable(short ray_index) = 0;
    virtual Count RayCount() const = 0;

    template<class Functor>
    void ForEachAppliedCast(Functor func) {
        for (auto i = 0; i < RayCount(); ++i) {
            CastRestorable(i);
            func(i);
            Restore();
        }
    }

    virtual ~Board_v2() = default;
};

class Board_v2_Reduce : public Board_v2 {
public:

    Count Cast(short ray_index) final {
        auto destroyed = CastImpl(ray_index);
        if (EmptySpace()/TotalSpace() > reduce_empty_ratio_) {
            Reduce();
        }
        return destroyed;
    }

    void set_reduce_empty_ratio(double ratio) {
        reduce_empty_ratio_ = ratio;
    }

protected:
    virtual Count CastImpl(short ray_index) = 0;
    virtual void Reduce() = 0;

    virtual Count EmptySpace() const = 0;
    virtual Count TotalSpace() const = 0;

private:
    // best parameter found out from benchmarking
    double reduce_empty_ratio_{0.63};
};
