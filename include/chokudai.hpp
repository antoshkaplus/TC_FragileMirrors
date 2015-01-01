//
//  chokudai.h
//  FragileMirrors
//
//  Created by Anton Logunov on 5/13/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __FragileMirrors__chokudai__
#define __FragileMirrors__chokudai__

#include <iostream>

#include "solver.hpp"


#define NMAX 128
#define NSHIFT 7
#define COUNTMAX 16000000

#define hashmax (1 << 24)

namespace chokudai {
    class FragileMirrors {
    public:
        vector<int> destroy(vector<string> board);
        vector<int> makeret();
        int getscore(int turn);
        void init(vector<string> b);
        void erase(int a, int b);
        void back();
        void inputerase(int a, int b, int num);
        void inputback();
        void lighterase(int a, int b);
        void lightback();
        
        int turnscore[200];
        int prenokori2[10000];
        long long prehash[10000];
        int starterase[NMAX][4];
    };
}

struct Chokudai : Solver{
    virtual vector<int> destroy(vector<string>& board) {
        chokudai::FragileMirrors fm;
        return fm.destroy(board);
    }
};


#endif /* defined(__FragileMirrors__chokudai__) */
