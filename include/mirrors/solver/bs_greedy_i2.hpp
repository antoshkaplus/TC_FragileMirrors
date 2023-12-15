#pragma once
#include <vector>
#include <ranges>
#include <unordered_set>
#include <map>
#include "mirrors/board/board_i2.hpp"


namespace mirrors {

// Traverse boards from best to worse. Should reduce number of inserts.
template <class Board>
class BS_Greedy_i2 {
public:
    explicit BS_Greedy_i2(size_t beam_width) : beam_width(beam_width) {}

    const Board& best_board() const {
        return boards.rbegin()->second;
    }

    std::vector<Position> Destroy(const Board& b) {
        std::vector<Position> cast_history;
        boards.emplace(0, b);
        while (!best_board().AllDestroyed()) {
            for (auto& item : std::views::reverse(boards)) {
                AddNextBoards(item.second);
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
            auto destroyed = cast_board.Cast(p);
            if (!visited.insert(cast_board.hash()).second) {
                return; // visited already
            }
            if (next_boards.size() < beam_width) {
                next_boards.emplace(destroyed, std::move(cast_board));
            } else if (next_boards.begin()->first < destroyed) {
                next_boards.erase(next_boards.begin());
                next_boards.emplace(destroyed, std::move(cast_board));
            }
        });
    }

private:
    size_t beam_width;

    std::unordered_set<hash_value_t> visited;
    std::multimap<cell_count_t, Board> next_boards;
    std::multimap<cell_count_t, Board> boards;
};

}