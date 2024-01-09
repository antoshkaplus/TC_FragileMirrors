/// this project is used to test new methods on real data
/// 


#include "_util.hpp"
#include "_score.hpp"
#include "_board_v1_impl_1.hpp"
#include "naive_search.hpp"
#include "nested_monte_carlo_search.hpp"
#include "board_v6.hpp"
#include "board_v5.hpp"
#include "board_v2_impl_1.hpp"
#include "beam_search.hpp"
#include "bs_balanced.hpp"
#include "_beam_search_history.hpp"
#include "bs_new.hpp"
#include "dfs.hpp"
#include "_stats.hpp"

using B_1 = Board_v1_Impl_1<CastHistory_Nodes>;




int main(int argc, const char * argv[]) {
    auto input = "./../temp/input.txt";
    auto output = "./../temp/output.txt";
    ifstream in(input);
    ofstream out(output);
    vector<string> board = GenerateStringBoard(50); // ReadBoard(in);
    Score_v1 s;
//    BeamSearchHistory<decltype(b), decltype(s)> solver;
//    BeamSearchNew<Board_v6> solver;
//    solver.set_millis(10000);
    int maxProblemSize = 400 * 100 * 100 * 4 * 100;
    int initWidth = maxProblemSize / (4*board.size()*board.size()*board.size());
    BeamSearchBalanced<Board_v6, Score_v1> solver;
    solver.set_time(std::chrono::seconds(100));
    solver.set_beam_width(initWidth);
    //solver.set_score(s);
    auto w = solver.Destroy(board);
    //LevelScoreDiff(solver, b, w);
    PrintSolution(out, w.CastHistory());
    //PrintSolution(std::cout, w.CastHistory());
    //Println(cout, solver.level_derivs_used());
    //auto& level_derivs = solver.level_derivs();
    //for_each(level_derivs.begin(), level_derivs.end(), [](auto& ds) { cout << ds.derivatives_left() << " "; });
    cout << endl;
}