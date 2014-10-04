//
//  Board.cpp
//  2013.tco.marathon.r.2
//
//  Created by Anton Logunov on 5/3/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include "GridBoard.h"

const unsigned char GridBoard::reflection[3][4] = {{0, 1, 2, 3}, {3, 2, 1, 0}, {1, 0, 3, 2}};
const unsigned char GridBoard::opposite[4] = {2, 3, 0, 1};


