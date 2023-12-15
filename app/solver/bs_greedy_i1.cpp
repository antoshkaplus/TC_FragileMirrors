#include <boost/program_options.hpp>
#include "mirrors/solver/bs_greedy_i1.hpp"
#include "mirrors/solver/bs_greedy_i2.hpp"
#include "mirrors/common/solver_util.hpp"
#include "mirrors/board/board_i2.hpp"
#include "mirrors/board/board_n_i1.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;

struct Params {
    size_t beam_width;
    std::string board_version;
    bool reverse;
};

Params parse_beam_width(const std::vector<std::string>& cmd_args) {
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
            ("board-version", po::value<std::string>(&params.board_version)->default_value("2"))
            ("reverse", po::bool_switch(&params.reverse));
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cs.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    std::cerr << "beam width: " << params.beam_width << '\n';
    std::cerr << "board version: " << params.board_version << '\n';
    return params;
}

template<template<class> class Solver>
std::vector<int> destroy(const Params& params, const mirrors::Grid<mirrors::Mirror>& gg) {
    if (params.board_version == "2") {
        mirrors::board::Board_i2 bb(gg);
        Solver<mirrors::board::Board_i2> solver(params.beam_width);
        auto cast_history = solver.Destroy(bb);
        return mirrors::ToSolution(cast_history);
    }
    if (params.board_version == "n1") {
        mirrors::Board_n_i1 bb(gg);
        Solver<mirrors::Board_n_i1> solver(params.beam_width);
        auto cast_history = solver.Destroy(bb);
        return mirrors::ToSolution(cast_history);
    }
    throw std::runtime_error("Unexpected board_version");
}

std::vector<int> FragileMirrors::destroy(const std::vector<std::string>& board) {
    auto params = parse_beam_width(cmd_args);
    auto gg = mirrors::ToMirrorsGrid(board);
    if (params.reverse) {
        return ::destroy<mirrors::BS_Greedy_i2>(params, gg);
    } else {
        return ::destroy<mirrors::solver::BS_Greedy_i1>(params, gg);
    }
}
