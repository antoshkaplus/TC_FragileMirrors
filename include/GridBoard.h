//
//  Board.h
//  2013.tco.marathon.r.2
//
//  Created by Anton Logunov on 5/3/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef ___013_tco_marathon_r_2__Board__
#define ___013_tco_marathon_r_2__Board__

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <numeric>
#include <cmath>

using namespace std;

struct Point {
  Point(){}
  Point(char row, char col) : row(row), col(col){}
  char row, col;
  void set(char row, char col){
    this->row = row; this->col = col;
  }
};


class GridBoardSolver;


class GridBoard {
  static const unsigned char VERTICAL = 0;
  static const unsigned char HORIZONTAL = 1;
  static const int MAX_SZ = 100+2;
  // E, R, L
  static const unsigned char reflection[3][4];
  static const unsigned char opposite[4];
  int shift[4];
  int mirrorsLeft;
  int gridSz;
  int possibleCastPositionsSz;
  int castPositionsSz;
  //int openedCastPositionsSz;
  //int lastInCastPosition;
  //int lastOutCastPosition;
  int totalDepth;
  unsigned char value[MAX_SZ*MAX_SZ];
  
  int possibleCastPositions[4*(MAX_SZ-2)];
  int castPositions[4*(MAX_SZ-2)];
  //int openedCastPositions[4*(MAX_SZ-2)];
  // 0 - vertical line, 1 - horizontal line
  unsigned char lineMirrorsLeft[2][MAX_SZ-2];
  
  GridBoard() {}
  
  int cast(int pos) {
    int k = 0;
    unsigned char dir = opposite[getBound(pos)];
    while (hasInside(pos+=shift[dir])) {
      unsigned char &g = value[pos];
      if (g) {
        lineMirrorsLeft[VERTICAL][pos%gridSz-1]--;
        lineMirrorsLeft[HORIZONTAL][pos/gridSz-1]--;
        dir = reflection[g][dir];
        g = 0;
        k++;        
      }
    }
    mirrorsLeft -= k;
    return k;
  }

  unsigned char getBound(int pos)const {
    return isBound0(pos) ? 0 : isBound1(pos) ? 1 : isBound2(pos) ? 2 : 3;
  }
  
  bool isBound0(int pos)const { return pos/gridSz == 0; }
  bool isBound1(int pos)const { return pos%gridSz == gridSz-1; }
  bool isBound2(int pos)const { return pos/gridSz == gridSz-1; }
  bool isBound3(int pos)const { return pos%gridSz == 0; }
  
  bool hasInside(int pos)const {
    return !(isBound0(pos) || isBound1(pos) || isBound2(pos) || isBound3(pos));
  }
  
  Point positionToPoint(int pos) {
    return Point(pos/gridSz-1, pos%gridSz-1);
  }
  
  // take odd k please for now
  vector<int> getNearestPositions(int pos, int q) {
    vector<int> res;
    unsigned char bound = getBound(pos);
    int index = bound*(gridSz-2)
        + (bound==0 ? pos%gridSz-1 :
           bound==1 ? pos/gridSz-1 :
           bound==2 ? gridSz-2-pos%gridSz : gridSz-2-pos/gridSz);
    for (int i = index-q; i <= index+q; i++) {
      res.push_back(castPositions[((i%castPositionsSz)+castPositionsSz)%castPositionsSz]);
    }
    return res;
  }  
  
  struct GreaterCast {
    bool operator()(const pair<int, int>& lhs, const pair<int, int>& rhs){
      return lhs.second > rhs.second;
      }
  };
  
  vector<int> getMostEffectiveCastsPositions(int q) {
    vector<int> res;
    vector< pair<int, int> > heap;
    int i, k;
    GridBoard gb;
    GreaterCast greater;
    for (i = 0; i < possibleCastPositionsSz;) {
      gb.copyFrom(*this);
      k = gb.cast(possibleCastPositions[i]);
      if (k == 0) {
        swap(possibleCastPositions[i], possibleCastPositions[--possibleCastPositionsSz]);
        continue;
      }
      
      // no HEAP, should do insertion !!!
      if (heap.size() < q) {
        heap.push_back(pair<int, int>(i, k));
        push_heap(heap.begin(), heap.end(), greater);
      }
      else if (k > heap.front().second) {
        pop_heap(heap.begin(), heap.end(), greater);
        heap.back() = pair<int, int>(i, k);
        push_heap(heap.begin(), heap.end(), greater);
      }
      i++;
    }
    for (i = 0; i < heap.size(); i++) {
      res.push_back(possibleCastPositions[heap[i].first]);
    }
    return res;
    
  }
  
public:
 
  void copyFrom(const GridBoard& gb) {
    memcpy(this, &gb, sizeof(GridBoard));
  }

  static GridBoard generate(int sz) {
    vector<string> strBoard(sz);
    for (int i = 0; i < sz; i++) {
      for (int j = 0; j < sz; j++) {
        strBoard[i].push_back(rand()%2 == 1 ? 'R' : 'L');
      }
    }
    return GridBoard(strBoard);
  }
  
  /* E -> 0
   * R -> 1
   * L -> 2
   */
  GridBoard(const vector<string>& b){
    gridSz = (int)b.size()+2;
    shift[0] = -gridSz;
    shift[1] = 1;
    shift[2] = gridSz;
    shift[3] = -1;
    castPositionsSz = possibleCastPositionsSz = 4*(gridSz-2);
    int k0, k1, k2, k3;
    int j0 = -1, j1 = gridSz-3, j2 = 2*gridSz-5, j3 = 3*gridSz-7;
    for (int i = 1; i < gridSz-1; i++) {
      value[k0=i] = value[k1=(i+1)*gridSz-1] = value[k2=gridSz*gridSz-i-1] = value[k3=(gridSz-i-1)*gridSz] = 0;
      castPositions[j0+i] = possibleCastPositions[j0+i] = k0;
      castPositions[j1+i] = possibleCastPositions[j1+i] = k1;
      castPositions[j2+i] = possibleCastPositions[j2+i] = k2;
      castPositions[j3+i] = possibleCastPositions[j3+i] = k3;
      lineMirrorsLeft[VERTICAL][i-1] = lineMirrorsLeft[HORIZONTAL][i-1] = gridSz;
    }
    for (int i = 1; i < gridSz-1; i++) {
      int k = i*gridSz;
      for (int j = 1; j < gridSz-1; j++) {
        value[k+j] = b[i-1][j-1] == 'R' ? 1 : 2;
      }
    }
    mirrorsLeft = (gridSz-2)*(gridSz-2);
  }
 
  bool canCast(){
    return mirrorsLeft != 0;
  }
      
  int getMirrorsLeft() { return mirrorsLeft; }
  int getPossibleCastPositionsSz() { return possibleCastPositionsSz; }
  int getBoardSize() { return gridSz-2; }
  ///int getOpenedCastPositionsSz() { return openedCastPositionsSz; }
  
  friend GridBoardSolver;
};


class GridBoardSolver : public GridBoard {  
  vector<Point> points;
  
 
  
public:
  GridBoardSolver(const vector<string>& strBoard) : GridBoard(strBoard){}
  
  const vector<Point>& getPoints() {
    return points;
  }
  
  void methodTwoSteps(){
    int i1, i2;
    int k1, k2;
    int maxK = 0, maxI = 0;
    GridBoard gb1, gb2;
    for (i1 = 0; i1 < possibleCastPositionsSz;) {
      gb1.copyFrom(*this);
      k1 = gb1.cast(possibleCastPositions[i1]);
      if (k1 == 0) {
        swap(possibleCastPositions[i1], possibleCastPositions[--possibleCastPositionsSz]);
        continue;
      }
      for (i2 = 0; i2 < possibleCastPositionsSz; i2++) {
        gb2.copyFrom(gb1);
        k2 = gb2.cast(possibleCastPositions[i2]);
        if (maxK < k1+k2) {
          maxI = i1;
          maxK = k1+k2;
        }
      }
      i1++;
    }
    cast(possibleCastPositions[maxI]);
    points.push_back(positionToPoint(possibleCastPositions[maxI]));
  }
  
  void methodOneStep() {
    int i, k;
    int maxK = 0, maxI = 0;
    for (i = 0; i < possibleCastPositionsSz;) {
      GridBoard db(*this);
      k = db.cast(possibleCastPositions[i]);
      if (k == 0) {
        swap(possibleCastPositions[i], possibleCastPositions[--possibleCastPositionsSz]);
        continue;
      }
      if (maxK < k) {
        maxI = i;
        maxK = k;
      }
      i++;
    }
    cast(possibleCastPositions[maxI]);
    points.push_back(positionToPoint(possibleCastPositions[maxI]));
  }
  
  
  void methodOneStepAmountOfEmptyLines(){
    int i, j;
    double crashed, amount;
    int maxCrashed = 0;
    int maxAmountOfEmptyLines = 0;
    int maxI = 0;
    GridBoard gb;
    vector<int> positions = getMostEffectiveCastsPositions(20);
    for (i = 0; i < positions.size(); i++) {
      gb.copyFrom(*this);
      crashed = gb.cast(positions[i]);
      amount = 0;
      for (j = 0; j < 2*(gridSz-2); j++) {
        if (lineMirrorsLeft[j] != 0 && gb.lineMirrorsLeft[j] == 0) amount++;
      }
      if (maxAmountOfEmptyLines < amount ||
         (maxAmountOfEmptyLines == amount && maxCrashed < crashed)) {
        maxI = i;
        maxAmountOfEmptyLines = amount;
        maxCrashed = crashed;
      }
    }
    cast(positions[maxI]);
    points.push_back(positionToPoint(positions[maxI]));
  }
  
  void methodTwoSteps2() {
    int i, i2;
    double crashed, crashed2;
    double maxCrashed2 = 0, maxCrashed = 0;
    int maxI = 0;
    GridBoard gb, gb2;
    vector<int> positions = getMostEffectiveCastsPositions(100);
    vector<int> positions2;
    for (i = 0; i < positions.size(); i++) {
      gb.copyFrom(*this);
      crashed = gb.cast(positions[i]);
      positions2 = gb.getMostEffectiveCastsPositions(1);
      crashed2 = 0;
      for (i2 = 0; i2 < positions2.size(); i2++) {
        gb2.copyFrom(gb);
        crashed2 += gb2.cast(positions2[i2]);
      }
      crashed2 /= 1;
      crashed2 += crashed;
      if (crashed2 > maxCrashed2 || (crashed2 == maxCrashed2 && crashed > maxCrashed)) {
        maxI = i;
        maxCrashed = crashed;
        maxCrashed2 = crashed2;
      }
    }
    cast(positions[maxI]);
    points.push_back(positionToPoint(positions[maxI]));
  }
  
  void methodTwoStepAmountOfEmptyLines(){
    int i1, i2, j1, j2;
    int crashed1, amount1;
    int crashed2, amount2;
    int maxCrashed = 0;
    int maxAmountOfEmptyLines = 0;
    int maxI = 0;
    GridBoard gb1, gb2;
    vector<int> positions1, positions2;
    positions1 = getMostEffectiveCastsPositions(100);
    for (i1 = 0; i1 < positions1.size(); i1++) {
      gb1.copyFrom(*this);
      crashed1 = gb1.cast(positions1[i1]);
      positions2 = gb1.getMostEffectiveCastsPositions(2);
      amount1 = 0;
      for (j1 = 0; j1 < 2*(gridSz-2); j1++) {
        if (lineMirrorsLeft[j1] != 0 && gb1.lineMirrorsLeft[j1] == 0) amount1++;
      }
      for (i2 = 0; i2 < positions2.size(); i2++) {
        gb2.copyFrom(gb1);
        crashed2 = gb2.cast(positions2[i2]);
        amount2 = 0;
        for (j1 = 0; j1 < 2*(gridSz-2); j1++) {
          if (gb1.lineMirrorsLeft[j1] != 0 && gb2.lineMirrorsLeft[j1] == 0) amount2++;
        }
      }
      if (maxAmountOfEmptyLines < amount1+amount2 || (maxAmountOfEmptyLines == amount1+amount2 && maxCrashed < crashed1+crashed2)) {
        maxI = i1;
        maxAmountOfEmptyLines = amount1+amount2;
        maxCrashed = crashed1+crashed2;
      }
    }
    cast(positions1[maxI]);
    points.push_back(positionToPoint(positions1[maxI]));
  }

};

#endif /* defined(___013_tco_marathon_r_2__Board__) */
