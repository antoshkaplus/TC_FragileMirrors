/// main program takes two arguments:
/// -i, -o
/// input and output file paths
/// input and output format are described in problem statement
/// -m : mehtod. for now just NaiveSearch
/// -s : score function probably

#include "util.hpp"
#include "score.hpp"
#include "board_v1.hpp"
#include "naive_search.hpp"
#include "beam_search.hpp"

int main(int argc, const char * argv[]) {
    command_line_parser parser(argv, argc);
    string input, output;
    if (parser.exists("i")) {
         input = parser.getValue("i");
    } else {
        cerr << "unable to find input file path";
        return 1;
    }
    if (parser.exists("o")) {
        output = parser.getValue("o");
    } else {
        cerr << "unable to find output file path";
        return 1;
    }
    
    ifstream in(input);
    ofstream out(output);
    vector<string> board = ReadBoard(in);
    Board_v1 b(board);
    Score_v1<Board_v1> s(board.size());
    BeamSearch<decltype(b), decltype(s)> solver;
    solver.set_beam_width(1000);
    //NaiveSearch<decltype(b), decltype(s)> solver;
    auto w = solver.Destroy(b, s);
    PrintSolution(out, w.CastHistory());
    return 0;
}