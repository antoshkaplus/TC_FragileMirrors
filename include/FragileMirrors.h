//
//  FragileMirrors.h
//  2013.tco.marathon.r.2
//
//  Created by Anton Logunov on 5/3/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef ___013_tco_marathon_r_2__FragileMirrors__
#define ___013_tco_marathon_r_2__FragileMirrors__

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

#include "solver.h"
#include "chokudai.h"
#include "eldidou.h"

using namespace std;


class FragileMirrors : Solver {
public:
  vector<int> destroy(vector<string>& board) override {
      //Chokudai ns;
      BeamSearchSolver ns;
      //IterativeSolver ns;
      return ns.destroy(board);
  }
};

#endif
