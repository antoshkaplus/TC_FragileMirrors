#include <string>
#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include "mirrors/board/board_n_i4.hpp"
#include "mirrors/board/board_n_i4_params.hpp"
#include "mirrors/solver/bs_restore_i3.hpp"
#include "mirrors/solver/bs_restore_i4.hpp"
#include "mirrors/common/solver_util.hpp"
#include "mirrors/score/score_c_i1.hpp"
#include "mirrors/score/score_c_i2.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;

struct Params {
    size_t solver_version;
    size_t beam_width;
    size_t score_version;
    std::vector<float> score_param; // use string and split.
};


inline std::vector<std::string> Split(std::string str, char delim) {
    std::vector<std::string> r;
    int s_i = 0; // starting index for sustr
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == delim) {
            r.push_back(str.substr(s_i, i-s_i));
            s_i = i+1;
        }
    }
    r.push_back(str.substr(s_i));
    return r;
}

Params parse_params(const std::vector<std::string>& cmd_args) {
    std::vector<const char*> cs;
    // Add mandatory first command line argument.
    cs.push_back("program");
    for(auto& arg: cmd_args) {
        cs.push_back(arg.c_str());
    }

    po::options_description desc;
    Params params;
    std::string score_param;
    desc.add_options()
            ("solver-version", po::value(&params.solver_version)->default_value(3))
            ("beam-width", po::value<size_t>(&params.beam_width)->default_value(300))
            ("score", po::value<size_t>(&params.score_version)->default_value(1))
            ("score-param", po::value<std::string>(&score_param)->default_value(",,,"));
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cs.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    std::cerr << "solver version: " << params.solver_version << '\n';
    std::cerr << "beam width: " << params.beam_width << '\n';
    std::cerr << "score version: " << params.score_version << '\n';
    if (vm.contains("score-param")) {
        auto items = Split(score_param, ',');
        for (auto& it : items) {
            if (it.empty()) it = "1.";
        }
        std::transform(items.begin(),
                       items.end(),
                       std::back_inserter(params.score_param),
                       [](const std::string& item) { return std::stof(item); });
        std::cerr << "score param: ";
        for (auto p: params.score_param) {
            std::cerr << p << " ";
        }
        std::cerr << '\n';
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

template<template <template<class> class, class, class> class Solver>
std::vector<int> destroy(const Params& params, OriginGrid<Grid<Mirror>>&ggg) {
    switch (params.score_version) {
        case 1: {
            Solver<mirrors::Score_c_i1, mirrors::Board_n_i4,
                    mirrors::Board_n_i4_Params> solver(params.beam_width, ggg);
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 2: {
            Score_c_i2<mirrors::Board_n_i4_Params> score;
            std::copy(params.score_param.begin(),
                      params.score_param.end(),
                      score.param.begin());
            Solver<mirrors::Score_c_i2, mirrors::Board_n_i4,
                    mirrors::Board_n_i4_Params> solver(params.beam_width, ggg, score);
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        default: {
            auto msg = boost::format("Unexpected score version: %1%") % params.score_version;
            throw std::runtime_error(msg.str());
        }
    }
}

}

std::vector<int> FragileMirrors::destroy(const std::vector<std::string>& board) {
    auto params = parse_params(cmd_args);
    auto gg = mirrors::ToMirrorsGrid(board);
    auto ggg = mirrors::MakeBoardGrid(gg);
    switch (params.solver_version) {
        case 3:
            return mirrors::destroy<mirrors::BS_Restore_i3>(params, ggg);
        default:
            auto msg = boost::format("Unexpected solver version: %1%") % params.score_version;
            throw std::runtime_error(msg.str());
    }
}
