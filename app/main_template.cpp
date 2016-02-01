/// main program takes two arguments:
/// -i, -o
/// input and output file paths
/// input and output format are described in problem statement
/// -m : mehtod. for now just NaiveSearch
/// -s : score function probably

#include "util.hpp"
#include "fragile_mirrors.hpp"


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
    FragileMirrors fm;
    auto v = fm.destroy(board);
    PrintSolution(out, v);
    return 0;
}