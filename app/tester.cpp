/// this project is used to test new methods on real data
/// 


#include "util.hpp"
#include "score.hpp"
#include "board_v1.hpp"
#include "naive_search.hpp"
#include "nested_monte_carlo_search.hpp"
#include "board_v6.hpp"
#include "board_v5.hpp"
#include "board_v4.hpp"
#include "beam_search.hpp"
#include "beam_search_history.hpp"
#include "dfs.hpp"
#include "stats.hpp"

using BoardType = Board_v1;

int main(int argc, const char * argv[]) {
    auto input = "./../temp/input.txt";
    auto output = "./../temp/output.txt";
    ifstream in(input);
    ofstream out(output);
    vector<string> board = ReadBoard(in);
    BoardType b(board);
    Score_v1<BoardType> s;
    BeamSearchHistory<decltype(b), decltype(s)> solver;
    solver.set_beam_width(1000);
    solver.set_score(s);
    auto w = solver.Destroy(b);
    LevelScoreDiff(solver, b, w);
    PrintSolution(out, w.CastHistory());


}