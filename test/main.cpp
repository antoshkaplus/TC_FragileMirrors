//
//  main.cpp
//  2013.tco.marathon.r.2
//
//  Created by Anton Logunov on 5/3/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "util.hpp"
#include "board_v1.hpp"

#include "gtest/gtest.h"


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

void printIntVector(const vector<int>& v) {
    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
    }
    cout << endl;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

