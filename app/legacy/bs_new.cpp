#include "_util.hpp"
#include "_score.hpp"

#include "board_v6.hpp"
#include "bs_new.hpp"
#include "fragile_mirrors.hpp"


BeamSearchNew<Board_v6> solver;

std::vector<int> FragileMirrors::destroy(const std::vector<std::string> & board) {
    solver.set_millis(10000);
    solver.set_beam_width(50 + 100 - board.size());
    auto w = solver.Destroy(board);
    return ToSolution(w.CastHistory());
}
