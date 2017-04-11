/// this project is used to test new methods on real data
/// 


#include "util.hpp"
#include "score.hpp"
#include "board_v1_impl_1.hpp"
#include "naive_search.hpp"
#include "nested_monte_carlo_search.hpp"
#include "board_v6.hpp"
#include "board_v5.hpp"
#include "board_v2_impl_1.hpp"
#include "beam_search.hpp"
#include "beam_search_history.hpp"
#include "beam_search_new.hpp"
#include "dfs.hpp"
#include "stats.hpp"

using BoardType = Board_v1_Impl_1<CastHistory_Nodes>;






int main(int argc, const char * argv[]) {
    auto input = "./../temp/input.txt";
    auto output = "./../temp/output.txt";
    ifstream in(input);
    ofstream out(output);
    vector<string> board = GenerateStringBoard(60); // ReadBoard(in);
    BoardType b(board);
    Score_v1 s;
//    BeamSearchHistory<decltype(b), decltype(s)> solver;
    //BeamSearch<BoardType, decltype(s)> solver;
    BeamSearchNew solver;
    solver.set_beam_width(100);
    solver.set_score(s);
    auto w = solver.Destroy(b);
    //LevelScoreDiff(solver, b, w);
    PrintSolution(out, w.CastHistory());
    Board_v2_Impl_1<CastHistory_Nodes> bb;

    Println(cout, solver.level_derivs_used());
    auto& level_derivs = solver.level_derivs();
    for_each(level_derivs.begin(), level_derivs.end(), [](auto& ds) { cout << ds.derivatives_left() << " "; });
    cout << endl;
}