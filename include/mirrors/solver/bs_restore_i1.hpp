#pragma once
#include <vector>
#include <unordered_set>
#include <set>
#include "mirrors/common/position.hpp"
#include "mirrors/score/less.hpp"
#include "mirrors/board/board_i5.hpp"
#include "mirrors/board/cast_i1.hpp"


namespace mirrors {

template <template<class> class Score>
class BS_Restore_i1 {
    using Board = Board_i5;
public:
    explicit BS_Restore_i1(size_t beam_width, Score<Board> score = Score<Board>()) : beam_width(beam_width),
                                                                                     less(score),
                                                                                     next_boards(less),
                                                                                     boards(less) {}

    const Board& best_board() const {
        return *boards.rbegin();
    }

    std::vector<Position> Destroy(const Board& b) {
        std::vector<Position> cast_history;
        boards.emplace(b);
        while (!best_board().AllDestroyed()) {
            for (auto& item : boards) {
                AddNextBoards(const_cast<Board&>(item));
            }
            boards.clear();
            boards.swap(next_boards);
            visited.clear();
        }
        return best_board().cast_history();
    }

    void AddNextBoards(Board& b) {
        b.ForEachCastCandidate([&](const Position& p) {
            Cast_i1 cast(b, p);
            if (!visited.insert(b.hash()).second) {
                return; // visited already
            }
            if (next_boards.size() < beam_width) {
                next_boards.emplace(b);
            } else if (less(*next_boards.begin(), b)) {
                next_boards.erase(next_boards.begin());
                next_boards.emplace(b);
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