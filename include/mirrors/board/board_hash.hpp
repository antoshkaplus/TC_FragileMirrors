#pragma once
#include "util.hpp"


class BoardHash {

    constexpr static size_t HashBitsCount = 64;
    using HashFunction = ZobristHashing<HashBitsCount>;
public:
    using HashType = typename HashFunction::value;


    BoardHash() {}

    BoardHash(Count sz) {
        hash_function_.reset(new HashFunction({sz, sz}, 1));
    }

    void HashIn(char row, char col) {
        HashIn({row, col});
    }

    void HashIn(const Position& p) {
        hash_function_->xorNothing(&hash_); // xor out
        hash_function_->xorState(&hash_, p, 0);
    }

    void HashOut(const Position& p) {
        hash_function_->xorState(&hash_, p, 0); // xor out
        hash_function_->xorNothing(&hash_); // xor in
    }

    const HashType& hash() const {
        return hash_;
    }

    void clear() {
        hash_ = 0;
    }

    bool empty() const {
        return hash_ == 0;
    }

    bool operator==(const BoardHash& bh) const {
        return bh.hash_ == hash_ && bh.hash_function_ == hash_function_;
    }

private:
    shared_ptr<HashFunction> hash_function_;
    HashType hash_;
};