#include "solver/greedy_i1.hpp"
#include "common/solver_util.hpp"
#include "fragile_mirrors.hpp"


std::vector<int> FragileMirrors::destroy(const std::vector<std::string>& board) {
    auto gg = mirrors::ToMirrorsGrid(board);
    mirrors::board::Board_i1 bb(gg);
    mirrors::solver::Greedy_i1 solver;
    auto cast_history = solver.Destroy(bb);
    return mirrors::ToSolution(cast_history);
}
