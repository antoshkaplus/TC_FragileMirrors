//
//  main.cpp
//  FragileMirrors
//
//  Created by Anton Logunov on 5/5/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <random>

#include "ant/optimization.h"


#include "beam_search.hpp"
#include "board_v1.hpp"
#include "board_v2.hpp"
#include "FragileMirrors.hpp"
#include "util.hpp"
#include "board_test.hpp"
#include "stats.hpp"
#include "best_first_search.hpp"
#include "greedy.hpp"
#include "board_v4.hpp"
#include "beam_search_v4.hpp"

using namespace ant;


vector<string> readBoard(istream& cin) {
    int n;
    cin >> n;
    vector<string> str_board(n);
    for (int i = 0; i < n; i++) {
        cin >> str_board[i];
    }
    return str_board;
}

void printSolution(ostream& cout, const vector<Position>& sol) {
    int n = 2 * sol.size();
    cout << n << endl;
    for (int i = 0; i < n; i++) {
        cout << sol[i].row << endl 
             << sol[i].col << endl;
    }

}

double score(int board_sz, int cast_count) {
    return board_sz / (double) cast_count;
}

//void TestCumulativeHistory(ostream& output) {
//    auto str_b = GenerateStringBoard(75);
//    Board b(str_b);
//    FragileMirrors fm;
//    cout << fm.destroy(str_b).size()/2 << endl;
//        
//    BeamSearch bs;
//    Board res = bs.destroy(b, 1000);
//    cout << res.History().size() << " " << res.mirrorsLeft() << endl;
//    auto history = res.CumulativeHistory();
//    for (int i = 0; i < history.size(); ++i) {
//        output << history[i] << " "; 
//    }
//}

void ResizeVector(vector<Count> &origin, const vector<Count>& model, Count def) {
    if (origin.size() < model.size()) {
        origin.resize(model.size(), def);
    }
}


// 
//void GatherDestroyedPerStep(ostream& output) {
//    output << "step,min,max,count,average" << endl; 
//    vector<Count> min_destroyed;
//    vector<Count> max_destroyed;
//    vector<Count> average;
//    vector<Count> count;
//    for (int i = 0; i < 10; ++i) {
//        auto str_b = GenerateStringBoard(75);
//        Board b(str_b);
//        BeamSearchSolver solver;
//        b = solver.Destroy(b, 500);
//        auto history = b.CumulativeHistory();
//        // removing leading 0
//        history.erase(history.begin());
//        ResizeVector(min_destroyed, history, numeric_limits<Count>::max());
//        ResizeVector(max_destroyed, history, 0);
//        ResizeVector(average, history, 0);
//        ResizeVector(count, history, 0);
//        for (int i = 0; i < history.size(); ++i) {
//            if (history[i] < min_destroyed[i]) min_destroyed[i] = history[i];
//            if (history[i] > max_destroyed[i]) max_destroyed[i] = history[i];
//            average[i] += history[i];
//            ++count[i]; 
//        }
//    }
//    for (int i = 0; i < count.size(); ++i) {
//        output  << i+1 << "," 
//                << min_destroyed[i] << "," 
//                << max_destroyed[i] << "," 
//                << count[i] << "," << 1.*average[i]/count[i] << endl;
//    }
//}
//



template <class Board, Count beam_width = 1000>
void TestBeamSearch(ostream& output) {
    auto str_b = GenerateStringBoard(75);
    Board b(str_b);
    ::BeamSearch solver;
    b = solver.Destroy(b, beam_width);
    output << b.CastCount();
}
//
//struct Params {
//    double destroyed_count;
//    double row_count;
//    double col_count;
//    
//    Params() : Params(0, 0, 0) {}
//    
//    Params(Count destroyed_count, Count row_count, Count col_count) 
//    : destroyed_count(destroyed_count), row_count(row_count), col_count(col_count) {}
//};
//
//using SolutionParams = vector<Params>;
//
//template<class Solver> 
//void CalculateStepParameters(ostream& output, Count sample_count, Count board_size, Count beam_width) {
//    vector<SolutionParams> solutions;
//    for (auto i = 0; i < sample_count; ++i) {
//        auto str_b = GenerateStringBoard(board_size);
//        Board b(str_b);
//        Solver solver;
//        b = solver.Destroy(b, beam_width);
//        SolutionParams s; 
//        while (true) {
//            b.restore();
//            s.emplace_back(b.mirrorsDestroyed(), board_size - b.emptyRowCount(), board_size - b.emptyColCount()); 
//            if (b.mirrorsDestroyed() == 0) break;
//        }
//        solutions.push_back(s);
//    }
//    Count sz = min_element(solutions.begin(), solutions.end(), [](const SolutionParams& s_0, const SolutionParams& s_1) {
//        return s_0.size() < s_1.size();
//    })->size();
//    vector<Params> mean(sz);
//    vector<Params> sd(sz);
//    for (int i = 0; i < sz; ++i) {
//        for (int k = 0; k < solutions.size(); ++k) {
//            mean[i].destroyed_count += solutions[k][i].destroyed_count;
//            mean[i].row_count += solutions[k][i].row_count;
//            mean[i].col_count += solutions[k][i].col_count;
//        }
//        mean[i].destroyed_count /= solutions.size();
//        mean[i].row_count /= solutions.size();
//        mean[i].col_count /= solutions.size();
//        
//        for (int k = 0; k < solutions.size(); ++k) {
//            sd[i].destroyed_count += pow(solutions[k][i].destroyed_count - mean[i].destroyed_count, 2);
//            sd[i].row_count += pow(solutions[k][i].row_count - mean[i].row_count, 2);
//            sd[i].col_count += pow(solutions[k][i].col_count - mean[i].col_count, 2);
//        }
//        sd[i].destroyed_count = sqrt(sd[i].destroyed_count / solutions.size());
//        sd[i].row_count = sqrt(sd[i].row_count / solutions.size());
//        sd[i].col_count = sqrt(sd[i].col_count / solutions.size());
//        
//        output << "step: " << i+1 << endl
//               << "mean: " << mean[i].destroyed_count << ", " << mean[i].row_count << ", " << mean[i].col_count << endl 
//               << "sd: " << sd[i].destroyed_count << ", " << sd[i].row_count << ", " << sd[i].col_count << endl;
//    }
//    
//    
//}
//
//
//

// probably is going to need special board for this stuff
void ComputeScoreFunction() {
    using Board = Board_v4;
    
    const int test_count = 200;
    const int board_size = 50;
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
            Greedy greedy;
            Board board(test_boards[i]);
            int cast_count = greedy.Destroy(board, func).CastCount();
            total_casts += cast_count;
        }
        cout << "computed at: " << variable << " casts: " << total_casts/test_count <<  endl;
        return total_casts;
    };
    
    double variable = opt::GoldenSectionSearch(0., board_size, objective, 0.01);
    cout << variable << endl;
}

void Test_V4() {
    using Board = Board_v4;
    Board b(GenerateStringBoard(75));
    BeamSearch_v4 bs;
    cout << bs.Destroy(b, 2000, 11.99, 1).CastCount();
    
    
}

void ComputeReduceFrequency() {
    auto str_board = GenerateStringBoard(75);
    Board_v4 b(str_board);
    auto objective = [&](double variable) {
        clock_t start = clock();
        BeamSearch_v4 bs;
        bs.Destroy(b, 200, 11.99, variable);
        clock_t result = clock() - start;
        cout << "at: " << variable << " clock: " << result << endl;
        return result;
    };
    opt::GoldenSectionSearch(0, 1, objective, 0.01);
}





int main(int argc, const char * argv[])
{
    if (argc == 1) {

        ComputeReduceFrequency();
        return 0;

        Test_V4();
        return 0;

        ComputeScoreFunction();
        return 0;

        //ifstream stat_in("stats.txt");
        //auto coeffs = ReadDestroyedCoeffs(stat_in);

        Chokudai ch;
        
        BeamSearch bs;
        auto board = GenerateStringBoard(75);
        //bs.Destroy_2(board, coeffs, 4000);
//        cout << "ch " <<  ch.destroy(board).size()/2 << endl;
//        cout << "my " << bs.Destroy(board, 6000, 25).CastCount() << endl;
        Greedy gr;
        BestFirstSearch bbs;
        bbs.Destroy(board);
        return 0;
        
//        ifstream stat_in("stats.txt");
//        auto coeffs = ReadDestroyedCoeffs(stat_in);
//        ofstream coeffs_out("coeffs.txt");
//        for (int i = 0; i < coeffs.size(); ++i) {
//            coeffs_out << i << " " << coeffs[i] << endl;
//        }
//        return 0;
        
        ofstream stat_out("stats.txt");
        Stats<Board_v1> stats;
        stats.Gather(1000, 75);
        stats.Print(stat_out);
        return 0;
        
        TestBeamSearch<Board_v1, 2000>(cout);
        return 0;
    }
//    Stats<abc::Board, BeamSearch> stats;
//    ofstream stats_out("sample_stats.txt");
//    stats.set_output(stats_out);
//    stats.Gather(10, 75);
//    return 0;
//     
//    TestAbcBoard();
//    return 0;
//
//    //TestBeamSolver<BeamSearch>(cout);
//    //FragileMirrors
//    //TestCumulativeHistory(cout);
////    ofstream output("stats.txt");
//    //CalculateStepParameters<BeamSearch>(cout, 10, 75, 100);
//    ofstream output("history.txt");
//    GatherDestroyedPerStep(output);
//    return 0;
//
//    FragileMirrors fg;
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
        BeamSearch solver;
        printSolution(out, solver.Destroy(str_board, 6000).HistoryCasts());
        return 0;
    }
//    if (parser.exists("test")) {
//        uniform_int_distribution<int> distr(50, 100);
//        default_random_engine rng;
//        double total_score = 0;
//        const int test_count = 100;
//        for (int i = 0; i < test_count; i++) {
//            auto board_sz = distr(rng);
//            auto str_board = GenerateStringBoard(board_sz);
//            auto casts = fg.destroy(str_board);
//            total_score += score(board_sz, (int)casts.size()/2);
//        }
//        cout << "score: " << total_score/test_count; 
//        return 0;
//    }
    return 0;
}

