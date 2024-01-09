//
//  stats.hpp
//  FRAGILE_MIRRORS
//
//  Created by Anton Logunov on 12/25/14.
//
// output format:
// step_index, example_count
// example_pars 1
// example_pars 2
// ...
//


#pragma once

#include <functional>

#include "beam_search_history.hpp"
#include "beam_search.hpp"
#include "board_v1_impl_1.hpp"
#include "score.hpp"
#include "board.hpp"
#include "util.hpp"
#include "cast_history.hpp"

using namespace std::placeholders;

template<class Board, size_t beam_width = 2000>
class Stats {
    
public:
    struct Example {
        Count destroyed_count;
        Count empty_line_count;
    };
    
    Count MaxSteps(const vector<Board>& bs) {
        return max_element(bs.begin(), bs.end(), [](const Board& b_0, const Board& b_1) {
            return b_0.CastCount() < b_1.CastCount();
        })->CastCount(); 
    }
    
    void Gather(Count sample_size, Count board_size) {
        vector<Board> boards;
//        for (int i = 0; i < sample_size; ++i) {
//            Board board(GenerateStringBoard(board_size));
//            BeamSearch solver;
//            board = solver.Destroy(board, beam_width);
//            boards.push_back(board);
//        }
        
        Count max_steps = MaxSteps(boards);
        sample_.resize(max_steps);
        for (auto& b : boards) {
            Board b_start = b.AfterCasts(0);
            int index = 0;
            for (auto& c : b.HistoryCasts()) {
                b_start.Cast(c);
                Example e;
                e.destroyed_count = b_start.MirrorsDestroyed();
                e.empty_line_count = b_start.EmptyLinesCount();
                sample_[index].push_back(e);
                ++index;
            }
        }
    }
    
    void Print(ostream& ou) {
        ou << sample_.size() << endl;
        for (int i = 0; i < sample_.size(); ++i) {
            auto& s = sample_[i];
            ou << i << " " << s.size() << endl;
            for (Example& e : s) {
                ou << e.destroyed_count << " " << e.empty_line_count << endl;
            } 
        }

    }
                
private:
    vector<vector<Example>> sample_;
};

vector<double> ReadDestroyedCoeffs(istream& in); 

// score diff between best and max

// we need to have some place to write it down
void LevelScoreDiff(BeamSearchHistory<Board_v1_Impl_1<CastHistory_Nodes>, Score_v1>& bs_history, Board_v1_Impl_1<CastHistory_Nodes> orig_board, Board_v1_Impl_1<CastHistory_Nodes>& sol) {
    auto& levels = bs_history.beam_levels();
    auto casts = sol.CastHistory();
    for (auto i = 0; i < casts.size(); ++i) {
        auto& beam = levels[i];
        auto fn = std::bind(&BeamSearchHistory<Board_v1_Impl_1<CastHistory_Nodes>, Score_v1>::score, bs_history, _1);
        auto max_board = MaxElement(beam.begin(), beam.end(), fn);
        auto min_board = MinElement(beam.begin(), beam.end(), fn);
        auto max_score = bs_history.score(*max_board);
        auto min_score = bs_history.score(*min_board);
        auto best_score = bs_history.score(orig_board);
        cout << max_score << " " << best_score << " " << min_score << endl;
        orig_board.Cast(casts[i]);
    }
};