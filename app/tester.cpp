/// this project is used to test new methods on real data
/// 


#include "util.hpp"
#include "score.hpp"
#include "board_v1.hpp"
#include "naive_search.hpp"
#include "beam_search.hpp"

int main(int argc, const char * argv[]) {
    auto input = "./../temp/input.txt";
    auto output = "./../temp/output.txt";
    ifstream in(input);
    ofstream out(output);
    vector<string> board = ReadBoard(in);
    Board_v1 b(board);
    Score<Board_v1> s;
    BeamSearch<decltype(b), decltype(s)> solver;
    solver.set_beam_width(1000);
    //NaiveSearch<decltype(b), decltype(s)> solver;
    auto w = solver.Destroy(b, s);
    PrintSolution(out, w.CastHistory());
}