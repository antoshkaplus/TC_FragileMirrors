//
//  main.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/5/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>


#include "board.h"
#include "FragileMirrors.h"

int main(int argc, const char * argv[])
{
  int n;
  cin >> n;
  vector<string> strBoard(n);
  for (int i = 0; i < n; i++) {
    cin >> strBoard[i];
  }
  FragileMirrors fg;
  vector<int> r = fg.destroy(strBoard);
  n = (int)r.size();
  cout << n << endl;
  for (int i = 0; i < n; i++) {
    cout << r[i] << endl;
  }
  return 0;
}

