//
//  optimizer.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 1/10/15.
//
//

#ifndef FRAGILE_MIRRORS_optimizer_hpp
#define FRAGILE_MIRRORS_optimizer_hpp

#include "util.hpp"

template<Count beam_width, class Board = Board_v6>
class Optimizer {
public:
    static Count BeamWidth(Count base_width, Count board_size) {
        return base_width * pow(100. / board_size, 2);
    }
    
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
                b = bs.Destroy(b, 
                               BeamWidth(beam_width, board_size), 
                               EMPTY_LINES_PARAM[board_size-50], 
                               1, 
                               variable);
                int cast_count = CastNode::Count(b.CastHistory());
                total_casts += cast_count;
            }
            cout << "computed at: " << variable << " casts: " << total_casts/test_count <<  endl;
            return total_casts + variable / 10000;
        };
        
        double variable = opt::GoldenSectionSearch(1., 4., objective, 0.1);
        cout << variable << endl;
        return variable;
    }
    
//    // optimize runtime
//    double ComputeReduceParam(Count test_count, Count board_size) {
//        vector<vector<string>> test_boards(test_count);
//        for (int i = 0; i < test_count; ++i) {
//            test_boards[i] = GenerateStringBoard(board_size);
//        }
//        
//        auto objective = [&](double variable) {
//            double time = clock();
//            for (int i = 0; i < test_count; ++i) {
//                BeamSearch_v2<Board> bs;
//                Board b(test_boards[i]);
//                CastNode::Count(bs.Destroy(b, 
//                                           BeamWidth(beam_width, board_size), 
//                                           EMPTY_LINES_PARAM[board_size-50], 
//                                           variable, 
//                                           OVERHEAD_PARAM[board_size-50]).CastHistory());
//            }
//            cout << "computed at: " << variable << " casts: " << (time = clock() - time) <<  endl;
//            return time;
//        };
//        
//        double variable = opt::GoldenSectionSearch(0., 10., objective, 0.1);
//        cout << variable << endl;
//        return variable;
//    }
//    
    
    
    // when we want to optimize everything :
    // EMPTY_LINES_PARAM is probably immutable
    // but we need something else for stability
    // so that overhead was OK
    
    //
};




#endif
