#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include "mirrors/board/board_n_i2.hpp"
#include "mirrors/solver/bs_restore_i3.hpp"
#include "mirrors/common/solver_util.hpp"
#include "mirrors/score/score_i1.hpp"
#include "mirrors/score/score_i2.hpp"
#include "mirrors/score/score_i3.hpp"
#include "mirrors/score/score_i4.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;

struct Params {
    size_t beam_width;
    size_t score_version;
    double score_param;
};

Params parse_params(const std::vector<std::string>& cmd_args) {
    std::vector<const char*> cs;
    // Add mandatory first command line argument.
    cs.push_back("program");
    for(auto& arg: cmd_args) {
        cs.push_back(arg.c_str());
    }

    po::options_description desc;
    Params params;
    desc.add_options()
            ("beam-width", po::value<size_t>(&params.beam_width)->default_value(300))
            ("score", po::value<size_t>(&params.score_version)->default_value(1))
            ("score-param", po::value<double>(&params.score_param));
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cs.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    std::cerr << "beam width: " << params.beam_width << '\n';
    std::cerr << "score version: " << params.score_version << '\n';
    if (vm.contains("score-param")) {
        std::cerr << "score param: " << params.score_param << '\n';
    }
    return params;
}

namespace mirrors {

inline OriginGrid<Grid<Mirror>> MakeBoardGrid(const Grid<Mirror> &mirrors_param) {
    OriginGrid<Grid<Mirror>> bg{{-1, -1}, mirrors_param.size() + 2};
    bg.grid().fill(Mirror::Border);
    for (auto row = 0; row < mirrors_param.size(); ++row) {
        for (auto col = 0; col < mirrors_param.size(); ++col) {
            bg[{row, col}] = mirrors_param(row, col);
        }
    }
    return bg;
}

}


std::vector<int> FragileMirrors::destroy(const std::vector<std::string>& board) {
    auto params = parse_params(cmd_args);
    auto gg = mirrors::ToMirrorsGrid(board);
    auto ggg = mirrors::MakeBoardGrid(gg);
    switch (params.score_version) {
        case 1: {
            mirrors::BS_Restore_i3<mirrors::Score_i1> solver(params.beam_width, ggg);
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 4: {
            mirrors::BS_Restore_i3<mirrors::Score_i4> solver(params.beam_width, ggg, {params.score_param});
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        default: {
            auto msg = boost::format("Unexpected score version: %1%") % params.score_version;
            throw std::runtime_error(msg.str());
        }
    }
}
