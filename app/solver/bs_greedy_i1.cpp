#include <boost/program_options.hpp>
#include "mirrors/solver/bs_greedy_i1.hpp"
#include "mirrors/common/solver_util.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;


size_t parse_beam_width(const std::vector<std::string>& cmd_args) {
    std::vector<const char*> cs;
    // Add mandatory first command line argument.
    cs.push_back("program");
    for(auto& arg: cmd_args) {
        cs.push_back(arg.c_str());
    }

    po::options_description desc;
    size_t beam_width;
    desc.add_options()
            ("beam-width", po::value<size_t>(&beam_width)->default_value(300));
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cs.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    std::cerr << "beam width: " << beam_width << '\n';
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
