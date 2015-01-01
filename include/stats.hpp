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


#ifndef FRAGILE_MIRRORS_stats_hpp
#define FRAGILE_MIRRORS_stats_hpp

#include "beam_search.hpp"
#include "board.hpp"
#include "util.hpp"

template<class Board = Board_v1, size_t beam_width = 2000>
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
        for (int i = 0; i < sample_size; ++i) {
            Board board(GenerateStringBoard(board_size));
            BeamSearch solver;
            board = solver.Destroy(board, beam_width);
            boards.push_back(board);
        }
        
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



#endif
