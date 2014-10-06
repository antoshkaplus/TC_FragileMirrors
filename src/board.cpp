//
//  board.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/7/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include "board.h"

const char Board::kDirTop      ;
const char Board::kDirBottom   ;
const char Board::kDirLeft     ;
const char Board::kDirRight    ;
const char Board::kDirNothing  ;

ostream& operator<<(ostream& output, const Board& board){
    for (auto row = 0; row < board.size(); ++row) {
        for (auto col = 0; col < board.size(); ++col) {
            output <<  (board.isDestroyed(row, col) ? 'o' : (board.mirror(row, col) == Board::kMirRight ? '\\' : '/'));
        }
        output << endl;
    }
    return output;
}

const char Board::kDirOpposite[];