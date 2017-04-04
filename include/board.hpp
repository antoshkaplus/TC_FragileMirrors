//
//  board.h
//  FragileMirrors
//
//  Created by Anton Logunov on 5/7/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#pragma once

#include <ant/core/core.hpp>

#include "util.hpp"


class Board {
public:

    virtual Count Cast(const Position& p) = 0;
    Count MirrorsLeft() const {
        return (Count)(Size()*Size() - MirrorsDestroyed());
    }
    virtual Count MirrorsDestroyed() const = 0;
    virtual bool IsDestroyed(Index row, Index col) const = 0;
    virtual Count Size() const = 0;
    virtual Count RowMirrorsLeft(Index row) const = 0;
    virtual Count ColMirrorsLeft(Index col) const = 0;
    
    virtual Count EmptyRowCount() const = 0;    
    virtual Count EmptyColCount() const = 0;
    
    virtual bool IsRowEmpty(Index row) const {
        return RowMirrorsLeft(row) == 0;
    }
    virtual bool IsColEmpty(Index col) const {
        return ColMirrorsLeft(col) == 0;
    }
    virtual Count EmptyLinesCount() const {
        return EmptyColCount() + EmptyRowCount();
    }
    
    virtual Count CastCount() const = 0;
    
    virtual ~Board() {}
};

ostream& operator<<(ostream& output, const Board& board);








