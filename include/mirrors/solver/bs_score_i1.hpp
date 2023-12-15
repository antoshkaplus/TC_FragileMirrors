#pragma once
#include <vector>
#include <ranges>
#include <unordered_set>
#include <set>
#include "mirrors/score/less.hpp"


namespace mirrors::solver {


template <template<class> class Score, class Board>
class BS_Score_i1 {
public:
    explicit BS_Score_i1(size_t beam_width, Score<Board> score = Score<Board>()) : beam_width(beam_width),
                                                                                   less(score),
                                                                                   next_boards(less),
                                                                                   boards(less) {}

    const Board& best_board() const {
        return *boards.rbegin();
    }

    std::vector<Position> Destroy(const Board& b) {
        boards.emplace(b);
        while (!best_board().AllDestroyed()) {
            for (auto& item : boards) {
                AddNextBoards(item);
            }
            boards.clear();
            boards.swap(next_boards);
            visited.clear();
        }
        return best_board().cast_history();
    }

    void AddNextBoards(const Board& b) {
        b.ForEachCastCandidate([&](const Position& p) {
            auto cast_board = b;
            cast_board.Cast(p);
            if (!visited.insert(cast_board.hash()).second) {
                return; // visited already
            }
            if (next_boards.size() < beam_width) {
                next_boards.emplace(std::move(cast_board));
            } else if (less(*next_boards.begin(), cast_board)) {
                next_boards.erase(next_boards.begin());
                next_boards.emplace(std::move(cast_board));
            }
        });
    }

private:
    size_t beam_width;
    Less<Score<Board>> less {};
    std::unordered_set<hash_value_t> visited;
    std::multiset<Board, Less<Score<Board>>> next_boards;
    std::multiset<Board, Less<Score<Board>>> boards;
};

}