//
//  FragileMirrors.cpp
//  2013.tco.marathon.r.2
//
//  Created by Anton Logunov on 5/3/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//
//
//#include "FragileMirrors.h"
//
//vector<int> FragileMirrors::destroy(vector<string>& strBoard){
//  vector<int> res;
//  GridBoardSolver b(strBoard);
//  time_t t1 = time(NULL);
//  
// // int sz = (int)strBoard.size()*(int)strBoard.size();
// // while (b.canCast() && b.getMirrorsLeft() > sz/3) {
// // b.methodTwoSteps2();
// // }
//  while (b.canCast()){// && b.getMirrorsLeft() > pow(strBoard.size(),2.)/3) {
//    b.methodTwoStepAmountOfEmptyLines();
//  }
//  
//  
//  
//  
////
////  while (b.canCast() && time(NULL)-t1 < 9) {
////    b.oneFromGlobalTwoStepCast();
////  }
////  while (b.canCast()) {
////    b.globalOneStepCast();
////  }
//  const vector<Point>& ps = b.getPoints();
//  for (int i = 0; i < ps.size(); i++) {
//    res.push_back(ps[i].row);
//    res.push_back(ps[i].col);
//  }
//  return res;
//}
//
