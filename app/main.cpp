//
//  main.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/5/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <random>
#include <iomanip>

#include "ant/optimization/optimization.h"

#include "beam_search.hpp"
#include "board_v1_impl_1.hpp"
#include "util.hpp"
#include "board_test.hpp"
#include "stats.hpp"
#include "best_first_search.hpp"
#include "greedy.hpp"
#include "board_v2_impl_1.hpp"
#include "beam_search_v4.hpp"
#include "board_v5.hpp"
#include "board_v6.hpp"
#include "optimizer.hpp"
#include "naive_search.hpp"

using namespace ant;



// probably is going to need special board for this stuff
template <class Board>
double ComputeEmptyLinesParam(Count test_count, Count board_size) {
    
    auto score_function = [](double v) {
        return [=](const Board board) {
            return - (board.MirrorsDestroyed() + v * board.EmptyLinesCount());
        };
    };
    
    vector<vector<string>> test_boards(test_count);
    for (int i = 0; i < test_count; ++i) {
        test_boards[i] = GenerateStringBoard(board_size);
    }
    
    auto objective = [&](double variable) {
        auto func = score_function(variable);
        double total_casts = 0;
        // try to create multiple threads here
        for (int i = 0; i < test_count; ++i) {
            Greedy<Board> greedy;
            Board board(test_boards[i]);
            int cast_count = CastNode::Count(greedy.Destroy(board, func).CastHistory());
            total_casts += cast_count;
        }
        cout << "computed at: " << variable << " casts: " << total_casts/test_count <<  endl;
        return total_casts;
    };
    
    double variable = opt::GoldenSectionSearch(0., 20., objective, 0.01);
    cout << variable << endl;
    return variable;
}

template<class Board>
void ComputeEvenLinesParam(Count test_count, Count board_size, double empty_lines_param) {
    
    auto score_function = [=](double v) {
        return [=](const Board board) {
            return - (board.MirrorsDestroyed() + empty_lines_param * board.EmptyLinesCount() + v * board.EvenLinesCount());
        };
    };
    
    vector<vector<string>> test_boards(test_count);
    for (int i = 0; i < test_count; ++i) {
        test_boards[i] = GenerateStringBoard(board_size);
    }
    
    auto objective = [&](double variable) {
        auto func = score_function(variable);
        double total_casts = 0;
        // try to create multiple threads here
        for (int i = 0; i < test_count; ++i) {
            Greedy<Board> greedy;
            Board board(test_boards[i]);
            int cast_count = CastNode::Count(greedy.Destroy(board, func).CastHistory());
            total_casts += cast_count;
        }
        cout << "computed at: " << variable << " casts: " << total_casts/test_count <<  endl;
        return total_casts;
    };
    
    double variable = opt::GoldenSectionSearch(0., 1., objective, 0.01);
    cout << variable << endl;
}

template <class Board>
double ComputeOverheadParam(Count test_count, Count board_size) {
    
    vector<vector<string>> test_boards(test_count);
    for (int i = 0; i < test_count; ++i) {
        test_boards[i] = GenerateStringBoard(board_size);
    }
    
    auto objective = [&](double variable) {
        double total_casts = 0;
        for (int i = 0; i < test_count; ++i) {
            BeamSearch_v2<Board> bs;
            Board b(test_boards[i]);
            int cast_count = CastNode::Count(bs.Destroy(b, 2000, EMPTY_LINES_PARAM[board_size-50], 1, variable).CastHistory());
            total_casts += cast_count;
        }
        cout << "computed at: " << variable << " casts: " << total_casts/test_count <<  endl;
        return total_casts + variable / 10000;
    };
    
    double variable = opt::GoldenSectionSearch(1., 4., objective, 0.1);
    cout << variable << endl;
    return variable;
}

// optimize runtime
template<class Board>
double ComputeReduceParam(Count test_count, Count board_size) {
    vector<vector<string>> test_boards(test_count);
    for (int i = 0; i < test_count; ++i) {
        test_boards[i] = GenerateStringBoard(board_size);
    }
    
    auto objective = [&](double variable) {
        double time = clock();
        for (int i = 0; i < test_count; ++i) {
            BeamSearch_v2<Board> bs;
            Board b(test_boards[i]);
            CastNode::Count(bs.Destroy(b, 2000, EMPTY_LINES_PARAM[board_size-50], variable, OVERHEAD_PARAM[board_size-50]).CastHistory());
        }
        cout << "computed at: " << variable << " casts: " << (time = clock() - time) <<  endl;
        return time;
    };
    
    double variable = opt::GoldenSectionSearch(0.2, 10., objective, 0.1);
    cout << variable << endl;
    return variable;
}
