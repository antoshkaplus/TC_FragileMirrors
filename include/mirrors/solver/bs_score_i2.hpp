#pragma once
#include <vector>
#include <ranges>
#include <unordered_set>
#include <boost/heap/priority_queue.hpp>
#include "mirrors/common/position.hpp"
#include "mirrors/score/greater.hpp"


namespace mirrors::solver {

// From BS_Score_i1. Use priority_queue instead of multimap to store boards between levels.
template <template<class> class Score, class Board>
class BS_Score_i2 {
public:
    explicit BS_Score_i2(size_t beam_width, Score<Board> score = Score<Board>()) : beam_width(beam_width),
                                                                                   greater(score),
                                                                                   next_boards(greater),
                                                                                   boards(greater) {}

    std::vector<Position> Destroy(const Board& b) {
        std::vector<Position> cast_history;
        boards.emplace(b);
        while(solution.empty()) {
            for (auto& item : boards) {
                AddNextBoards(item);
            }
            boards.clear();
            boards.swap(next_boards);
            visited.clear();
        }
        return solution;
    }

    void AddNextBoards(const Board& b) {
        b.ForEachCastCandidate([&](const Position& p) {
            auto cast_board = b;
            cast_board.Cast(p);
            if (cast_board.AllDestroyed() && solution.empty()) {
                solution = cast_board.cast_history();
                return;
            }
            if (!visited.insert(cast_board.hash()).second) {
                return; // visited already
            }
            if (next_boards.size() < beam_width) {
                next_boards.emplace(std::move(cast_board));
            } else if (greater(cast_board, next_boards.top())) {
                next_boards.pop();
                next_boards.emplace(std::move(cast_board));
            }
        });
    }

private:
    size_t beam_width;
    Greater<Score<Board>> greater {};
    std::unordered_set<hash_value_t> visited;
    boost::heap::priority_queue<Board, boost::heap::compare<Greater<Score<Board>>>> next_boards;
    boost::heap::priority_queue<Board, boost::heap::compare<Greater<Score<Board>>>> boards;
    std::vector<Position> solution;
};

}