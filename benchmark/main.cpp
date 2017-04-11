//
// Created by Anton Logunov on 4/11/17.
//
#include "benchmark/benchmark_api.h"


int main(int argc, char** argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}
