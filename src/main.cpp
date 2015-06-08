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
#include "board_v5.hpp"
#include "board_v6.hpp"
#include "optimizer.hpp"

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

// base is better make 1000 for faster testing purposes
// need to add beam_width = base * (100 / board_size)^2

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



template<class Board>
void Test_V4() {
    Board b(GenerateStringBoard(75));
    BeamSearch_v2<Board> bs;
    cout << CastNode::Count(bs.Destroy(b, 2000, 11.99, 1, 2.).CastHistory());
}


template<class Board_0, class Board_1>
void CompareBoards() {
    auto ss = GenerateStringBoard(75);
    Board_0 b_0(ss);
    Board_1 b_1(ss);
    // b5 is better optimized
    clock_t ck_0 = clock();
    BeamSearch_v2<Board_0> bs_0;
    cout << CastNode::Count(bs_0.Destroy(b_0, 2000, 11.99, 1).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;
    ck_0 = clock();
    BeamSearch_v2<Board_1> bs_1;
    cout << CastNode::Count(bs_1.Destroy(b_1, 2000, 11.99, 1).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;
}

template<class Board>
void CompareReduction() {
    auto ss = GenerateStringBoard(75);
    Board r_half(ss), r_one(ss), r_three(ss);
    clock_t ck_0 = clock();
    BeamSearch_v2<Board> bs_0;
    cout << CastNode::Count(bs_0.Destroy(r_half, 2000, 11.99, 1).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;
    ck_0 = clock();
    BeamSearch_v2<Board> bs_1;
    cout << CastNode::Count(bs_1.Destroy(r_one, 2000, 11.99, 10000000).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;
}

template<class Board> 
void CompareSwap() {
    auto ss = GenerateStringBoard(75);
    Board r_half(ss), r_one(ss), r_three(ss);
    clock_t ck_0 = clock();
    BeamSearch_v2<Board> bs_0;
    cout << CastNode::Count(bs_0.Destroy(r_half, 2000, 11.99, 1).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;
    ck_0 = clock();
    BeamSearch_v2<Board> bs_1;
    //cout << CastNode::Count(bs_1.Destroy_2(r_one, 2000, 11.99, 1).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;

}


template<class Board>
void CompareReduce() {
    auto ss = GenerateStringBoard(75);
    Board r_half(ss), r_one(ss), r_three(ss);
    clock_t ck_0 = clock();
    BeamSearch_v2<Board> bs_0;
    cout << CastNode::Count(bs_0.Destroy(r_half, 2000, 11.99, 0.5).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;
    ck_0 = clock();
    BeamSearch_v2<Board> bs_1;
    cout << CastNode::Count(bs_1.Destroy(r_one, 2000, 11.99, 10000).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;
    ck_0 = clock();
    BeamSearch_v2<Board> bs_2;
    cout << CastNode::Count(bs_2.Destroy(r_three, 2000, 11.99, 1).CastHistory()) << endl;
    cout << clock() - ck_0 << endl;
}

//void ComputeReduceFrequency() {
//    auto str_board = GenerateStringBoard(90);
//    Board_v4 b(str_board);
//    auto objective = [&](double variable) {
//        clock_t start = clock();
//        BeamSearch_v2<Board_v4> bs;
//        bs.Destroy(b, 200, 8.24, variable, 1.5);
//        clock_t result = clock() - start;
//        cout << "at: " << variable << " clock: " << result << endl;
//        return result;
//    };
//    double variable = opt::GoldenSectionSearch(0, 1, objective, 0.005);
//    cout << variable << endl;
//}


class FragileMirrors {
public:
    vector<int> destroy(vector<string>& board)  {
        Board_v6 b(board);
        BeamSearch_v2<Board_v6> bs;
        b = bs.Destroy(b, 
                       500 * pow(100. / b.size(), 2), 
                       EMPTY_LINES_PARAM[b.size()-50], 
                       OVERHEAD_PARAM[b.size()-50]);
        auto ps = CastNode::ToHistory(b.CastHistory());
        vector<int> res;
        for (auto& p : ps) {
            res.push_back(p.row);
            res.push_back(p.col);
        }
        return res;
    }
};


int main(int argc, const char * argv[])
{
    if (argc == 1) {

//        CompareReduce<Board_v4>();
//        return 0;
//
//        Test_V4<Board_v6>();
//        for (int i = 0; i < 10; ++i) {
//            CompareSwap<Board_v6>();
//            //CompareReduction<Board_v6>();
//            //CompareBoards<Board_v6, Board_v4>();
//        }
//        return 0;
//  
//        vector<double> params;
//        Optimizer<500> optimizer;
//        double d = optimizer.ComputeReduceParam(10, 50);

//        for (Count c = 50; c <= 100; ++c) {
//            //double d = ComputeEmptyLinesParam<Board_v6>(20, c);
//            double d = optimizer.ComputeReduceParam(30, c);
//            //double d = ComputeReduceParam<Board_v6>(30, c);
//            params.push_back(d);
//        }
        
        for (int k = 50; k <= 100; ++k) {
            auto b = GenerateStringBoard(k);
            FragileMirrors fm;
            auto r = fm.destroy(b);
            cout << b.size() << endl;
            for (int i = 0; i < r.size()/2; ++i) {
                cout << r[2*i] << ", " << r[2*i + 1] << endl;
            }
            cout << endl << endl << endl;
        }
        
        
        return 0;

        //ifstream stat_in("stats.txt");
        //auto coeffs = ReadDestroyedCoeffs(stat_in);

        Chokudai ch;
        
        BeamSearch bs;
        auto board = GenerateStringBoard(75);
        //bs.Destroy_2(board, coeffs, 4000);
//        cout << "ch " <<  ch.destroy(board).size()/2 << endl;
//        cout << "my " << bs.Destroy(board, 6000, 25).CastCount() << endl;
        //Greedy gr;
        auto func = [](Board_v2& b) {
            return b.MirrorsDestroyed();
        };
        BestFirstSearch<Board_v2, decltype(func)> bbs;
        bbs.set_score(func);
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
        FragileMirrors solver;
     //   printSolution(out, solver.destroy(str_board));
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

