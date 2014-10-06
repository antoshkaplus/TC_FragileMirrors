//
//  main.cpp
//  2013.tco.marathon.r.2
//
//  Created by Anton Logunov on 5/3/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "FragileMirrors.h"
#include "GridBoard.h"

using namespace std;

vector<string> generateStringBoard(int sz) {
  vector<string> strBoard(sz);
  for (int i = 0; i < sz; i++) {
    for (int j = 0; j < sz; j++) {
      strBoard[i].push_back(rand()%2 == 1 ? 'R' : 'L');
    }
  }
  return strBoard;
}

void printStringBoard(const vector<string>& b) {
  for (int i = 0; i < b.size(); i++) {
    cout << b[i] << endl;
  }
  cout << endl;
}

void printPoints(const vector<Point>& ps) {
  for (int i = 0; i < ps.size(); i++) {
    cout << "(" << int(ps[i].row) << ", " << int(ps[i].col) << ")" << endl;
  }
  cout << endl;
}

void printIntVector(const vector<int>& v) {
  for (int i = 0; i < v.size(); i++) {
    cout << v[i] << " ";
  }
  cout << endl;
}

void testFragileMirrors(){
  FragileMirrors fm;
  vector<string> b = generateStringBoard(100);
  vector<int> r = fm.destroy(b);
  for (int i = 0; i < r.size(); i++) {
    cout << r[i] << endl;
  }
}

void testSubmission() {
  double res = 0.;
  int n;
  for (int  i = 0; i < 100; i++) {
    n = rand()%51+50;
    FragileMirrors fm;
    vector<string> strBoard = generateStringBoard(n);
    res += (double)2*n/fm.destroy(strBoard).size();
  }
  cout << res << endl;
}

void testSolutions() {
  const int t[5] = {51, 61, 71, 81, 91};
  const int testQuantity = 100;
  
  ofstream out("globalOneBestTwoStepCast.txt");
  for (int i = 0; i < 5; ++i) {
    cout << "start " << t[i] << endl;
    // number of crashed mirrors at that number of mirrors left
    vector<int> res(t[i]+9, 0);
    for (int j = 0; j < testQuantity; ++j) {
      GridBoardSolver b(generateStringBoard(rand()%10+t[i]));
      time_t tt = time(NULL);
      int k = -1;
      int mirrorsLeft = b.getBoardSize()*b.getBoardSize();
      while (b.canCast() && time(NULL)-tt < 9 && ++k < t[i]+9) {
        //b.globalOneBestTwoStepCast();
        res[k] += mirrorsLeft-b.getMirrorsLeft();
      }
    }
    out << i << endl;
    for (int j = 0; j < res.size(); ++j) {
      out << "(" << j << "," << res[j] << ") ";
    }
    out << endl;
  }
}

void testGridBoards() {
  ofstream out("compareGridBoards.txt");
  for (int i = 0; i < 100; i++) {
    int sz = rand()%51+50;
    GridBoard gb(GridBoard::generate(sz));
    int k1 = 0;
    time_t t = time(NULL);
    while (gb.canCast() && time(NULL)-t < 5) {
      
      k1++;
    }
    GridBoard ogb(GridBoard::generate(sz));
    int k2 = 0;
    t = time(NULL);
    while (ogb.canCast() && time(NULL)-t < 5) {
      k2++;
    }
    out << sz << " new: " << k1 << ", old: " << k2 << endl;
  }
}

void testStrategy() {
  ofstream out("strategyName.txt");
  
}

int main(int argc, const char * argv[])
{
  testSubmission();
//  auto strs = generateStringBoard(10);
//  FragileMirrors fm;
//  fm.destroy(strs);
  return 0;
}

