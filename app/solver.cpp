/// main program takes two arguments:
/// -i, -o
/// input and output file paths
/// input and output format are described in problem statement
/// -m : mehtod. for now just NaiveSearch
/// -s : score function probably

#include "util.hpp"
#include "score.hpp"

#include "board_v1_impl_1.hpp"
#include "board_v6.hpp"

#include "bs_new.hpp"
#include "naive_search.hpp"
#include "beam_search.hpp"
#include "nested_monte_carlo_search.hpp"

#include "fragile_mirrors.hpp"

std::vector<int> FragileMirrors::destroy(const std::vector<std::string> & board) {
    BeamSearchNew<Board_v6> solver;
    solver.set_millis(8000);
    solver.set_beam_width(100);
    auto w = solver.Destroy(board);
    return ToSolution(w.CastHistory());
}
