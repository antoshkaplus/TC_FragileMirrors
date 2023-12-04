#include <boost/program_options.hpp>
#include "solver/bs_greedy_i1.hpp"
#include "common/solver_util.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;


size_t parse_beam_width(const std::vector<std::string>& cmd_args) {
    std::vector<const char*> cs;
    for(auto& arg: cmd_args) {
        cs.push_back(arg.c_str());
    }

    po::options_description desc;
    size_t beam_width;
    desc.add_options()
            ("beam-width", po::value<size_t >(&beam_width)->default_value(300));
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cmd_args.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    return beam_width;
}


std::vector<int> FragileMirrors::destroy(const std::vector<std::string>& board) {
    auto beam_width = parse_beam_width(cmd_args);
    auto gg = mirrors::ToMirrorsGrid(board);
    mirrors::board::Board_i2 bb(gg);
    mirrors::solver::BS_Greedy_i1 solver(beam_width);
    auto cast_history = solver.Destroy(bb);
    return mirrors::ToSolution(cast_history);
}
