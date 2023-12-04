#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include "common/timer.hpp"
#include "common/solver_util.hpp"
#include "fragile_mirrors.hpp"


namespace po = boost::program_options;

/// main program takes two arguments:
/// -i, -o
/// input and output file paths
/// input and output format are described in problem statement
/// -m : mehtod. for now just NaiveSearch
/// -s : score function probably
int main(int argc, const char * argv[]) {
    po::options_description desc;
    std::string input, output;
    desc.add_options()
        ("timed,t", "timed")
        ("input,i", po::value<std::string>(&input), "input file")
        ("output,o", po::value<std::string>(&output), "output file");

    po::variables_map vm;
    auto parsed_options = po::command_line_parser(argc, argv).
            options(desc).
            allow_unregistered().
            run();
    po::store(parsed_options, vm);
    po::notify(vm);

    std::vector<std::string> other_cmd_args = po::collect_unrecognized(parsed_options.options, po::exclude_positional);

    bool timed = false;
    if (vm.contains("timed")) {
        timed = true;
    }

    std::istream *in = &std::cin;
    std::ostream *out = &std::cout;

    std::ifstream fin;
    std::ofstream fout;

    if (vm.contains("input")) {
        std::cerr << "input from file " << input << std::endl;
        fin.open(input);
        if (fin.fail()) {
            std::cerr << "input file can not be open" << std::endl;
            return 1;
        }
        in = &fin;
    }
    if (vm.contains("output")) {
        std::cerr << "output from file" << std::endl;
        fout.open(output);
        if (fout.fail()) {
            std::cerr << "output file can not be open" << std::endl;
            return 1;
        }
        out = &fout;
    }

    std::vector<std::string> board = mirrors::ReadBoard(*in);
    
    mirrors::Timer timer(10000);
    FragileMirrors fm;
    fm.cmd_args = other_cmd_args;
    auto v = fm.destroy(board);
    if (timed && timer.timeout()) {
        return 0;
    }
    mirrors::PrintSolution(*out, v);
    return 0;
}