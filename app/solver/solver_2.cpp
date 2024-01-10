#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include "mirrors/board/board_r6.hpp"
#include "mirrors/solver/bs_restore_i7.hpp"
#include "mirrors/solver/solver_2.hpp"
#include "mirrors/solver/solver_3.hpp"
#include "mirrors/solver/solver_4.hpp"
#include "mirrors/solver/solver_5.hpp"
#include "mirrors/common/solver_util.hpp"
#include "mirrors/score/score_i1.hpp"
#include "mirrors/score/score_i4.hpp"
#include "mirrors/score/score_i5.hpp"
#include "mirrors/score/score_i6.hpp"
#include "mirrors/score/score_i9.hpp"
#include "mirrors/score/score_i10.hpp"
#include "mirrors/score/score_i11.hpp"
#include "mirrors/score/score_psyho.hpp"
#include "mirrors/param/empty_lines_param.hpp"
#include "mirrors/param/beam_width.hpp"
#include "legacy/score_.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;

struct Params {
    size_t solver_version;
    size_t beam_width_version;
    size_t beam_width;
    double beam_width_param;
    size_t score_version;
    double score_param;
    std::string board_version;
};

struct BeamWidthOptions {
    BeamWidthOptions(po::options_description& desc, Params& params) : params(params) {
        desc.add_options()
                ("beam-width-version", po::value(&params.beam_width_version)->default_value(1))
                ("beam-width", po::value(&beam_width)->default_value("300"))
                // needed only for version 3
                ("beam-width-param", po::value(&params.beam_width_param)->default_value(0));
    }

    void parse(mirrors::board_size_t board_size) {
        std::cerr << "beam width version: " << params.beam_width_version << '\n';

        if (beam_width == "10sec") {
            params.beam_width = mirrors::_10_SEC_FIXED_WIDTH[board_size-50];
        } else if (beam_width =="10sec2") {
            params.beam_width = mirrors::_10_SEC_FIXED_WIDTH_2[board_size-50];
        } else if (beam_width =="20sec2") {
            params.beam_width = 2*mirrors::_10_SEC_FIXED_WIDTH_2[board_size-50];
        } else {
            params.beam_width = std::stoul(beam_width);
        }
        std::cerr << "beam width: init: " << params.beam_width;

        if (params.beam_width_version == 3) {
            std::cerr << " param: " << params.beam_width_param;
        }
        std::cerr <<  '\n';
    }

    Params& params;
    std::string beam_width {};
};


Params parse_params(const std::vector<std::string>& cmd_args, mirrors::board_size_t board_size) {
    std::vector<const char*> cs;
    // Add mandatory first command line argument.
    cs.push_back("program");
    for(auto& arg: cmd_args) {
        cs.push_back(arg.c_str());
    }

    po::options_description desc;
    Params params {};
    std::string score_param;
    desc.add_options()
            ("solver-version", po::value(&params.solver_version)->default_value(3))
            ("score", po::value<size_t>(&params.score_version)->default_value(4))
            ("score-param", po::value<std::string>(&score_param)->default_value("empty-lines"))
            ("board", po::value(&params.board_version));
    BeamWidthOptions bw_options(desc, params);
    po::variables_map vm;
    auto parsed_options = po::parse_command_line(cs.size(), cs.data(), desc);
    po::store(parsed_options, vm);
    po::notify(vm);

    std::cerr << "solver version: " << params.solver_version << '\n';
    bw_options.parse(board_size);
    std::cerr << "score version: " << params.score_version << '\n';
    const auto kMinBoardSize = 50;
    if (score_param == "legacy") {
        params.score_param = legacy::EMPTY_LINES_PARAM[board_size-kMinBoardSize];
    } else if (score_param == "empty-lines") {
        params.score_param = mirrors::EMPTY_LINES_PARAM[board_size-kMinBoardSize];
    } else if (score_param == "empty-lines-10") {
        params.score_param = mirrors::EMPTY_LINES_PARAM_10[board_size-kMinBoardSize];
    } else {
        params.score_param = std::stod(score_param);
    }
    std::cerr << "score param: " << params.score_param << '\n';

    if (vm.contains("board")) {
        std::cerr << "board version: " << params.board_version << '\n';
    }
    return params;
}

namespace mirrors {

template<template <template<class> class, class, class> class Solver, class Board>
std::vector<int> destroy(const Params& params, Grid<mir_t>& ggg) {
    switch (params.score_version) {
        case 1: {
            Solver<mirrors::Score_i1, Board, Board_n_i2_Params> solver(params.beam_width, ggg);
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 4: {
            Solver<mirrors::Score_i4, Board, Board_n_i2_Params> solver(params.beam_width, ggg, {params.score_param});
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 5: {
            Solver<mirrors::Score_i5, Board, Board_n_i2_Params> solver(params.beam_width, ggg);
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 6: {
            Solver<mirrors::Score_i6, Board, Board_n_i2_Params> solver(params.beam_width, ggg, {params.score_param});
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 9: {
            Solver<mirrors::Score_i9, Board, Board_n_i2_Params> solver(params.beam_width, ggg, {params.score_param});
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 10: {
            Score_i10<Board_n_i2_Params> score(ggg.size(), params.score_param);
            Solver<mirrors::Score_i10, Board, Board_n_i2_Params> solver(params.beam_width, ggg, score);
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 11: {
            Score_i11<Board_n_i2_Params> score(ggg.size(), params.score_param);
            Solver<mirrors::Score_i11, Board, Board_n_i2_Params> solver(params.beam_width, ggg, score);
            auto cast_history = solver.Destroy();
            return mirrors::ToSolution(cast_history);
        }
        case 100: {
            Solver<mirrors::Score_Psyho, Board, Board_n_i2_Params> solver(params.beam_width, ggg, {ggg.size()});
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
    auto params = parse_params(cmd_args, board.size());
    auto gg = mirrors::ToMirGrid(board);
    switch (params.solver_version) {
        case 2: return mirrors::destroy<mirrors::Solver_2, mirrors::Board_r6>(params, gg);
        case 3: return mirrors::destroy<mirrors::Solver_3, mirrors::Board_r6>(params, gg);
        case 4: return mirrors::destroy<mirrors::Solver_4, mirrors::Board_r6>(params, gg);
        case 5: return mirrors::destroy<mirrors::Solver_5, mirrors::Board_r6>(params, gg);
        default:
            auto msg = boost::format("Unexpected solver version: %1%") % params.solver_version;
            throw std::runtime_error(msg.str());
    }

}
