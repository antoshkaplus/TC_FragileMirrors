#pragma once
#include <vector>
#include <unordered_set>
#include <set>
#include <ranges>
#include "mirrors/common/position.hpp"
#include "mirrors/score/less.hpp"
#include "mirrors/board/board_i5.hpp"
#include "mirrors/board/board_i5_restore_cast.hpp"
#include "mirrors/board/board_i5_cast.hpp"


namespace mirrors {

// From BS_Score_i1. Add AddNextBoardsRestore - use temporal cast on a board
// when selecting best boards. So that we don't copy before each cast but do that
// only when want to add to best boards set.
// This does not help with performance at any level of the beam search.
template <template<class> class Score>
class BS_Restore_i1 {
    using Board = Board_i5;
public:
    explicit BS_Restore_i1(size_t beam_width,
                           double restore_destroyed_ratio,
                           Score<Board> score = Score<Board>()) :
            beam_width(beam_width),
            restore_destroyed_ratio(restore_destroyed_ratio),
            less(score),
            next_boards(less),
            boards(less) {}

    const Board& best_board() const {
        return *boards.rbegin();
    }

    std::vector<Position> Destroy(const Board& b) {
        boards.emplace(b);
        double total_cells = b.cell_count();
        while (!best_board().AllDestroyed() ) {
            if (best_board().destroyed_count() / total_cells < restore_destroyed_ratio) {
                for (auto &item: boards) {
                    AddNextBoardsRestore(const_cast<Board&>(item));
                }
            } else {
                for (auto& item : boards) {
                    AddNextBoards(item);
                }
            }
            boards.clear();
            boards.swap(next_boards);
            visited.clear();
        }
        return best_board().cast_history();
    }

    void AddNextBoardsRestore(Board& b) {
        b.ForEachCastCandidate([&](const Position& p) {
            RestoreCast cast(b, p);
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

    void AddNextBoards(const Board& b) {
        b.ForEachCastCandidate([&](const Position& p) {
            auto cast_board = b;
            Cast(cast_board, p);
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
    double restore_destroyed_ratio;
    Less<Score<Board>> less {};
    std::unordered_set<hash_value_t> visited;
    std::multiset<Board, Less<Score<Board>>> next_boards;
    std::multiset<Board, Less<Score<Board>>> boards;
};

}