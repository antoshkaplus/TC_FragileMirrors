// -sz : board size
// -min_w : minimum width bound
// -max_w : maximum width bound
// -n : number of boards to test
// -s : seconds per solution
#include "ant/core/core.hpp"

#include "beam_search.hpp"
#include "board_v6.hpp"
#include "_score.hpp"


template<class Board, class Solver>
int ComputeMaxWidth(const Board& b, Solver& s, int minWidth, int maxWidth) {
	auto cond = [&](int width) {
		s.set_beam_width(width);
		auto r = s.Destroy(b);
		return r.CastCount() != 0;
	};
	return ant::LogicalBinarySearch<int, decltype(cond)>::Max(minWidth, maxWidth, cond);
}

int main(int argc, const char * argv[]) {
	command_line_parser parser(argv, argc);
	int sz = ant::atoi(parser.getValue("sz"));
	int minWidth = ant::atoi(parser.getValue("min_w"));
	int maxWidth = ant::atoi(parser.getValue("max_w"));
	int boardCountPerCase = 1;
	if (parser.exists("n")) {
		boardCountPerCase = atoi(parser.getValue("n"));
	}
	std::chrono::seconds time{10};
	if (parser.exists("s")) {
		time = decltype(time){atoi(parser.getValue("s"))};
	}

	ant::Stats stats;
	for (size_t i = 0; i < boardCountPerCase; ++i) {
		BeamSearch<Board_v6, Score_v1> solver;
		solver.set_time(time);
		Board_v6 orig = GenerateStringBoard(sz);
		stats.add(ComputeMaxWidth(orig, solver, minWidth, maxWidth));
	}
	Println(std::cout, "Mix width: ", (int)stats.min());
	Println(std::cout, "Max width: ", (int)stats.max());
	Println(std::cout, "Ave width: ", (int)stats.average());
}
