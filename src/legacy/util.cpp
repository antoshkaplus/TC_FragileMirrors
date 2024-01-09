//
//  util.cpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/25/14.
//
//

#include <random>

#include "_util.hpp"

#ifdef RNG

default_random_engine RNG(std::chrono::system_clock::now().time_since_epoch().count());

#else

default_random_engine RNG;

#endif

vector<string> GenerateStringBoard(int sz) {
    discrete_distribution<int> distr{0.5, 0.5};
    vector<string> strBoard(sz);
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < sz; j++) {
            strBoard[i].push_back(distr(RNG) == 0 ? 'R' : 'L');
        }
    }
    return strBoard;
}

vector<string> ReadBoard(istream& cin) {
    int n;
    cin >> n;
    vector<string> str_board(n);
    for (int i = 0; i < n; i++) {
        cin >> str_board[i];
    }
    return str_board;
}

void PrintSolution(ostream& cout, const vector<Position>& sol) {
    int n = 2 * sol.size();
    cout << n << endl;
    for (auto& p : sol) {
        cout << p.row << endl 
        << p.col << endl;
    }    
}

void PrintSolution(ostream& cout, const vector<int>& sol) {
    cout << sol.size() << endl;
    for (auto i : sol) {
        cout << i << endl;
    }
}

std::vector<int> ToSolution(const std::vector<Position>& ps) {
    vector<int> res;
    res.reserve(2*ps.size());
    for (auto& p : ps) {
        res.push_back(p.row);
        res.push_back(p.col);
    }
    return res;
}

void PrintStringBoard(const vector<string>& b) {
    for (int i = 0; i < b.size(); i++) {
        cout << b[i] << endl;
    }
    cout << endl;
}

StrBoard ConvertToLetterMirrors(StrBoard b) {
    auto func = [&](Position p) {
        b[p.row][p.col] = (b[p.row][p.col] == '/') ? 'L' : 'R';
    };
    Region(0, 0, b.size(), b.size()).ForEach(func);
    return b;
}
