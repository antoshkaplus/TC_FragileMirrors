#include "util.hpp"
#include "score.hpp"
#include "board_v6.hpp"
#include "bs_balanced.hpp"
#include "fragile_mirrors.hpp"


std::vector<int> FragileMirrors::destroy(const std::vector<std::string> & board) {
    BeamSearchBalanced<Board_v6, Score_v1> solver;
    solver.set_time(std::chrono::seconds(100));
    solver.set_beam_width(board.size());
    auto w = solver.Destroy(board);
    return ToSolution(w.CastHistory());
}
