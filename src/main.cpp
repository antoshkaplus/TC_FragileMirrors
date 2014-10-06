//
//  main.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/5/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
<<<<<<< HEAD
#include <random>
=======

>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250

#include "board.h"
#include "FragileMirrors.h"

<<<<<<< HEAD


vector<string> generateStringBoard(int sz) {
    vector<string> strBoard(sz);
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < sz; j++) {
            strBoard[i].push_back(rand()%2 == 1 ? 'R' : 'L');
        }
    }
    return strBoard;
}

vector<string> readBoard(istream& cin) {
    int n;
    cin >> n;
    vector<string> str_board(n);
    for (int i = 0; i < n; i++) {
        cin >> str_board[i];
    }
    return str_board;
}

void printSolution(ostream& cout, const vector<int>& sol) {
    int n = (int)sol.size();
    cout << n << endl;
    for (int i = 0; i < n; i++) {
        cout << sol[i] << endl;
    }

}

double score(int board_sz, int cast_count) {
    return board_sz / (double) cast_count;
}


int main(int argc, const char * argv[])
{
    FragileMirrors fg;
    ant::command_line_parser parser(argv, argc);
    if (parser.exists("solve")) {
        string input = "input.txt";
        if (parser.exists("i")) {
            input = parser.getValue("i");
        }
        string output = "output.txt";
        if (parser.exists("o")) {
            output = parser.getValue("o");
        }
        ifstream in(input);
        ofstream out(output);
        
        auto str_board = readBoard(in);
        printSolution(out, fg.destroy(str_board));
        return 0;
    }
    if (parser.exists("test")) {
        uniform_int_distribution<int> distr(50, 100);
        default_random_engine rng;
        double total_score = 0;
        const int test_count = 100;
        for (int i = 0; i < test_count; i++) {
            auto board_sz = distr(rng);
            auto str_board = generateStringBoard(board_sz);
            auto casts = fg.destroy(str_board);
            total_score += score(board_sz, (int)casts.size()/2);
        }
        cout << "score: " << total_score/test_count; 
        return 0;
    }
    return 0;
=======
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
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
}

