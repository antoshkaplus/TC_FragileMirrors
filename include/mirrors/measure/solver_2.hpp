#pragma once
#include <vector>
#include <unordered_set>
#include <set>
#include <ranges>
#include <span>
#include "mirrors/common/position.hpp"
#include "mirrors/common/grid.hpp"
#include "mirrors/common/origin_grid.hpp"
#include "mirrors/common/zobrist_hashing.hpp"
#include "mirrors/score/less.hpp"

namespace mirrors {

struct CandidateDuplicates {
    size_t beam_width {};
    size_t count {};
    size_t candidate_count {};
};


template <template<class> class Score, class Board, class BoardParams, class BeamWidth>
class Solver_2 {
    using ScoreValue = Score<BoardParams>::Value;
    using CastValue = Board::CastValue;
    using BoardGrid = Grid<mir_t>;

    struct Derivative {
        BoardParams params;
        const Board* board;
        ScoreValue score;
        CastValue cast;

        bool operator<(const Derivative& other) const {
            return score > other.score || (score == other.score && params.hash < other.params.hash);
        }
    };

    struct BoardItem {
        Board board;
        BoardParams params;
    };

public:
    explicit Solver_2(BeamWidth beam_width,
                      const BoardGrid& mirrors,
                      Score<BoardParams> score = Score<BoardParams>()) :
            beam_width(beam_width),
            mirrors(mirrors),
            less(score) {}

    std::vector<Position> Destroy() {
        int size = mirrors.size();
        hashing = ZobristHashing(size);
        boards.emplace_back(Board(size), BoardParams{size});
        for (auto i = 0; i < size*size; ++i) {
            for (auto& item: boards) {
                AddBoardDerivatives(item);
            }
            auto res = AddDerivativesNextBoards();
            if (res) {
                return res.value();
            }
            derivs.clear();
            boards.clear();
            boards.swap(next_boards);
        }
        throw std::runtime_error("not found");
        return {};
    }

    void AddBoardDerivatives(BoardItem& item) {
        item.board.ForEachCastCandidate([&](const CastValue& p) {
            auto new_params = item.board.RestoreCast(
                    p, item.params, mirrors, hashing);
            derivs.emplace_back(new_params, &item.board, less.score(new_params), p);
        });
    }

    std::optional<std::vector<Position>> AddDerivativesNextBoards() {
        auto empty_lines = 0;
        for (auto& d : derivs) {
            empty_lines = std::max(static_cast<int>(d.params.empty_lines()),
                                   empty_lines);
        }
        auto bw = beam_width.width(empty_lines);
        CandidateDuplicates duplicates {bw};
        duplicates.candidate_count = 6*bw;
        auto end = derivs.begin()+std::min(derivs.size(),
                                           duplicates.candidate_count);

        std::nth_element(derivs.begin(),
                         end-1,
                         derivs.end());
        std::sort(derivs.begin(), end);
        hash_value_t last_hash = 0;
        for (auto& d: std::span(derivs.begin(), end)) {
            if (d.params.hash == last_hash) {
                ++duplicates.count;
                continue;
            }
            last_hash = d.params.hash;
            auto b = *d.board;
            b.Cast(d.cast, mirrors);
            if (d.params.board_empty(b)) {
                return b.cast_history();
            }
            next_boards.emplace_back(std::move(b), d.params);
            if (next_boards.size() == bw) {
                break;
            }
        }

        candidate_duplicates_.push_back(duplicates);
        return {};
    }

    const std::vector<CandidateDuplicates>& candidate_duplicates() const {
        return candidate_duplicates_;
    }

private:
    BeamWidth beam_width;
    ZobristHashing hashing;
    const BoardGrid& mirrors;
    Less<Score<BoardParams>> less {};
    std::vector<BoardItem> next_boards;
    std::vector<BoardItem> boards;
    std::vector<Derivative> derivs;

    std::vector<CandidateDuplicates> candidate_duplicates_;
};

}