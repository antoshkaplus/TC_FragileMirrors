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

// Based on BS_Restore_i1. Instead of adding straight to next level boards,
// we create a big set of all candidates and then pick the most prominent ones,
// Create next_boards set with them.
// Did not see performance benefits.
template <template<class> class Score>
class BS_Restore_i2 {
    using Board = Board_i5;
    using ScoreValue = Score<Board>::Value;

    struct Derivative {
        const Board* board;
        ScoreValue score;
        Position cast;

        bool operator<(const Derivative& other) const {
            return score > other.score;
        }
    };
public:
    explicit BS_Restore_i2(size_t beam_width,
                           double restore_destroyed_ratio,
                           Score<Board> score = Score<Board>()) :
            beam_width(beam_width),
            restore_destroyed_ratio(restore_destroyed_ratio),
            less(score) {}

    const Board& best_board() const {
        return boards.rbegin()->second;
    }

    std::vector<Position> Destroy(const Board& b) {
        boards.emplace(less.score(b), b);
        double total_cells = b.cell_count();
        while (!best_board().AllDestroyed() ) {
            if (best_board().destroyed_count() / total_cells < restore_destroyed_ratio) {
                for (auto& item : boards) {
                    AddNextBoards(item.second);
                }
            } else {
                for (auto& item: boards) {
                    AddBoardDerivatives(item.second);
                }
                AddDerivativesNextBoards();
            }
            boards.clear();
            boards.swap(next_boards);
            visited.clear();
        }
        return best_board().cast_history();
    }

    void AddBoardDerivatives(Board& b) {
        b.ForEachCastCandidate([&](const Position& p) {
            RestoreCast cast(b, p);
            if (!visited.insert(b.hash()).second) {
                return; // visited already
            }
            derivs.emplace_back(&b, less.score(b), p);
        });
    }

    void AddDerivativesNextBoards() {
        auto end = derivs.begin()+std::min(derivs.size(), beam_width);
        std::nth_element(derivs.begin(),
                         end-1,
                         derivs.end());
        std::for_each(derivs.begin(), end, [&](const Derivative& d) {
            Board& b = next_boards.emplace(d.score, *d.board)->second;
            Cast(b, d.cast);
        });
        derivs.clear();
    }


    void AddNextBoards(const Board& b) {
        b.ForEachCastCandidate([&](const Position& p) {
            auto cast_board = b;
            Cast(cast_board, p);
            if (!visited.insert(cast_board.hash()).second) {
                return; // visited already
            }
            if (next_boards.size() < beam_width) {
                next_boards.emplace(less.score(cast_board), std::move(cast_board));
            } else if (next_boards.begin()->first < less.score(cast_board)) {
                next_boards.erase(next_boards.begin());
                next_boards.emplace(less.score(cast_board), std::move(cast_board));
            }
        });
    }

private:
    size_t beam_width;
    double restore_destroyed_ratio;
    Less<Score<Board>> less {};
    std::unordered_set<hash_value_t> visited;
    std::multimap<ScoreValue, Board> next_boards;
    std::multimap<ScoreValue, Board> boards;
    std::vector<Derivative> derivs;
};

}