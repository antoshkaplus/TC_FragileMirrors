/// main program takes two arguments:
/// -i, -o
/// input and output file paths
/// input and output format are described in problem statement
/// -m : mehtod. for now just NaiveSearch
/// -s : score function probably

#include "util.hpp"
#include "score.hpp"
#include "board_v1.hpp"
#include "naive_search.hpp"
#include "beam_search.hpp"
#include "fragile_mirrors.hpp"
#include "nested_monte_carlo_search.hpp"

std::vector<int> FragileMirrors::destroy(const std::vector<std::string> & board) {
    Board_v1 b(board);
    Score_v1<Board_v1> s;
    BeamSearch<decltype(b), decltype(s)> solver;
    solver.set_beam_width(1000);
    //NaiveSearch<decltype(b), decltype(s)> solver;
    auto w = solver.Destroy(b, s);
    return ToSolution(w.CastHistory());
}
