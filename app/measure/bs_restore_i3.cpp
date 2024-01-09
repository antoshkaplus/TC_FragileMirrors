#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include "mirrors/board/board_n_i2.hpp"
#include "mirrors/board/board_n_i3.hpp"
#include "mirrors/measure/bs_restore_i3.hpp"
#include "mirrors/common/solver_util.hpp"
#include "mirrors/score/score_i4.hpp"
#include "mirrors/param/empty_lines_param.hpp"
#include "mirrors/param/beam_width.hpp"
#include "legacy/score_.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;

struct Params {
    size_t solver_version;
    size_t beam_width;
    size_t score_version;
    double score_param;
};


Params parse_params(const std::vector<std::string>& cmd_args, mirrors::board_size_t board_size) {
    std::vector<const char*> cs;
    // Add mandatory first command line argument.
    cs.push_back("program");
    for(auto& arg: cmd_args) {
        cs.push_back(arg.c_str());
    }

    po::options_description desc;
    Params params;
    std::string beam_width;
    std::string score_param;
    desc.add_options()
            ("solver-version", po::value(&params.solver_version)->default_value(3))
            ("beam-width", po::value<std::string>(&beam_width)->default_value("300"))
            ("score", po::value<size_t>(&params.score_version)->default_value(4))
            ("score-param", po::value<std::string>(&score_param));
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cs.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    std::cerr << "solver version: " << params.solver_version << '\n';
    if (beam_width == "10sec") {
        params.beam_width = mirrors::_10_SEC_FIXED_WIDTH[board_size-50];
    } else {
        params.beam_width = std::stoul(beam_width);
    }

    std::cerr << "beam width: " << params.beam_width << '\n';
    std::cerr << "score version: " << params.score_version << '\n';
    const auto kMinBoardSize = 50;
    if (vm.contains("score-param")) {
        if (score_param == "legacy") {
            params.score_param = legacy::EMPTY_LINES_PARAM[board_size-kMinBoardSize];
        } else {
            params.score_param = std::stod(score_param);
        }
    } else {
        params.score_param = mirrors::EMPTY_LINES_PARAM[board_size-kMinBoardSize];
    }
    std::cerr << "score param: " << params.score_param << '\n';
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

template<template <template<class> class, class, class> class Solver, class Board>
std::vector<int> destroy(const Params& params, OriginGrid<Grid<Mirror>>&ggg) {
    switch (params.score_version) {
        case 4: {
            Solver<mirrors::Score_i4, Board, Board_n_i2_Params> solver(params.beam_width, ggg, {params.score_param});
            auto cast_history = solver.Destroy();

            auto& lev_scores = solver.level_scores();
            for (auto i = 0; i < lev_scores.size(); ++i) {
                std::cerr << i << '\n';
                auto& min = lev_scores[i].first;
                std::cerr << "  min:" << min.score << " d:" << min.params.destroyed_count() << " lines:" << min.params.empty_lines() << '\n';
                auto& max = lev_scores[i].second;
                std::cerr << "  max:" << max.score << " d:" << max.params.destroyed_count() << " lines:" << max.params.empty_lines() << '\n';
            }

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
    auto params = parse_params(cmd_args, board.size());
    auto gg = mirrors::ToMirrorsGrid(board);
    auto ggg = mirrors::MakeBoardGrid(gg);
    switch (params.solver_version) {
        case 3:
            return mirrors::destroy<mirrors::BS_Restore_i3, mirrors::Board_n_i2>(params, ggg);
        default:
            auto msg = boost::format("Unexpected solver version: %1%") % params.score_version;
            throw std::runtime_error(msg.str());
    }
}
