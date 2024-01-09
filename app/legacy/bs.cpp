#include "_util.hpp"
#include "_score.hpp"
#include "board_v6.hpp"
#include "beam_search.hpp"
#include "fragile_mirrors.hpp"


std::vector<int> FragileMirrors::destroy(const std::vector<std::string> & board) {
    BeamSearch<Board_v6, Score_v1> solver;
    solver.set_time(std::chrono::seconds(100));
    solver.set_beam_width(500.*pow(100./board.size(), 2));
    auto w = solver.Destroy(board);
    return ToSolution(w.CastHistory());
}
