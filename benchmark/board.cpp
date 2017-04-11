//
// Created by Anton Logunov on 4/11/17.
//
#include "benchmark/benchmark_api.h"

#include "beam_search.hpp"
#include "board_v2_impl_1.hpp"
#include "board_v5.hpp"
#include "board_v6.hpp"


using B_1 = Board_v2_Impl_1<CastHistory_Nodes>;
using B_2 = Board_v5;
using B_3 = Board_v6;


template <class B>
static void BeamSearchBenchmark(benchmark::State& state) {
    BeamSearch<B, Score_v1> bs;
    bs.set_beam_width(1000);
    RNG.seed(0);
    B b = GenerateStringBoard(state.range(0));
    while (state.KeepRunning()) {
        bs.Destroy(b);
    }
}

BENCHMARK_TEMPLATE(BeamSearchBenchmark, B_1)->Arg(50)->Arg(100);
BENCHMARK_TEMPLATE(BeamSearchBenchmark, B_2)->Arg(50)->Arg(100);
BENCHMARK_TEMPLATE(BeamSearchBenchmark, B_3)->Arg(50)->Arg(100);


template <class B>
static void BoardCastBenchmark(benchmark::State& state) {
    RNG.seed(0);
    B b = GenerateStringBoard(state.range(0));
    while (!b.AllDestroyed()) {
        uniform_int_distribution<> ray_distr(0, b.RayCount()-1);
        while (state.KeepRunning()) {
            b.Cast(ray_distr(RNG));
        }
    }
}

BENCHMARK_TEMPLATE(BoardCastBenchmark, B_1)->Arg(50)->Arg(100);
BENCHMARK_TEMPLATE(BoardCastBenchmark, B_2)->Arg(50)->Arg(100);
BENCHMARK_TEMPLATE(BoardCastBenchmark, B_3)->Arg(50)->Arg(100);



