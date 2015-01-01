//
//  solver.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/7/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include "solver.hpp"

//MultiDerivative Derivate(shared_ptr<Board> board, Position cast) {
//    MultiDerivative deriv(board, cast);
//    board->cast(cast);
//    CastIterator it(board->size());
//    while (it.HasNext()) {
//        Position p = it.Next();
//        board->cast(p);
//        auto& last_cast = board->LastCast();
//        bool must_cast = false;
//        if (last_cast.size() == 1) {
//            if (board->LinkCount(*last_cast.begin()) == 0) {
//                must_cast = true;
//            }
//        } else if (last_cast.size() == 2) {
//            must_cast = true;
//            for (auto& p : last_cast) {
//                if (board->LinkCount(p) != 1) {
//                    // should not do
//                    must_cast = false;
//                    break;
//                }
//            }
//            // must do if nothing happened
//        }
//        if (!must_cast) board->restore();
//        else {
//            deriv.casts.push_front(p);
//        }
//    }
//    deriv.destroy_count = board->mirrorsDestroyed();
//    for (auto& c : deriv.casts) {
//        (void)c;
//        board->restore();
//    }
//    return deriv;
//}
