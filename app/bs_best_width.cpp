#include "ant/core/core.hpp"

#include "beam_search.hpp"
#include "board_v6.hpp"
#include "score.hpp"


template<class Board, class Solver>
int ComputeMaxWidth(const Board& b, Solver& s, int minWidth, int maxWidth) {
	auto cond = [&](int width) {
		s.set_beam_width(width);
		auto r = s.Destroy(b);
		return r.CastCount() != 0;
	};
	return ant::LogicalBinarySearch<int, decltype(cond)>::Max(minWidth, maxWidth, cond);
}

int main() {
	const int kBoardCountPerCase = 1;
    std::chrono::seconds kTime{10};


	int sz, minWidth, maxWidth;
	std::cin >> sz;
	std::cin >> minWidth >> maxWidth;

	ant::Stats stats;
	for (size_t i = 0; i < kBoardCountPerCase; ++i) {
		BeamSearch<Board_v6, Score_v1> solver;
		solver.set_time(kTime);
		Board_v6 orig = GenerateStringBoard(sz);
		stats.add(ComputeMaxWidth(orig, solver, minWidth, maxWidth));
	}
	Println(std::cout, "Max width: ", stats.average());
}
