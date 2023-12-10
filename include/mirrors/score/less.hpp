#pragma once

namespace mirrors {

template<class Score>
struct Less {
    bool operator()(const Score::Board& b_1, const Score::Board& b_2) const {
        return score(b_1) < score(b_2);
    }

    Score score {};
};

}