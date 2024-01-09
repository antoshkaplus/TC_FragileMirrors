#include "_util.hpp"
#include "_score.hpp"
#include "board_v6.hpp"
#include "bs_balanced.hpp"
#include "fragile_mirrors.hpp"


std::vector<int> FragileMirrors::destroy(const std::vector<std::string> & board) {
    int maxProblemSize = 400 * 100 * 100 * 4 * 100;
    int initWidth = maxProblemSize / (4*board.size()*board.size()*board.size());
    BeamSearchBalanced<Board_v6, Score_v1> solver;
    solver.set_time(std::chrono::seconds(100));
    solver.set_beam_width(initWidth);
    auto w = solver.Destroy(board);
    return ToSolution(w.CastHistory());
}
