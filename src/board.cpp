//
//  board.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/7/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include "board.hpp"
//
//const int Board::kDirTop      ;
//const int Board::kDirBottom   ;
//const int Board::kDirLeft     ;
//const int Board::kDirRight    ;
//const int Board::kDirNothing  ;
//
//ostream& operator<<(ostream& output, const Board& board){
//    for (auto row = 0; row < board.size(); ++row) {
//        for (auto col = 0; col < board.size(); ++col) {
//            output <<  (board.isDestroyed(row, col) ? 'o' : (board.mirror(row, col) == Board::kMirRight ? '\\' : '/'));
//        }
//        output << endl;
//    }
//    return output;
//}
//
//const int Board::kDirOpposite[];
//
//namespace ant {
//namespace grid {
//    
//bool operator!=(const Position& p_0, const Position& p_1) {
//    return p_0.row != p_1.row || p_0.col != p_1.col;
//}
//
//} 
//}