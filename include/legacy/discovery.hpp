//
// Created by Anton Logunov on 4/12/17.
//
#pragma once

#include "util.hpp"
#include "board.hpp"


struct Discovery {

    bool Discover(const Board& b) {
        return discovered_.insert(b.hash()).second;
    }

private:

    unordered_set<Board::HashType> discovered_;
};

struct DiscoveryNew {

    bool Discover(const Board& b) {
        auto it = discovered_.find(b.hash());
        if (it == discovered_.end() || it->second > b.CastCount()) {
            discovered_[b.hash()] = b.CastCount();
            return true;
        }
        return false;
    }

private:
    // look for less count of cast on discover
    unordered_map<Board::HashType, Count> discovered_;
};


// result is little bit worse because we lose board state info with lower amount of bits
struct DiscoveryFast {
    static constexpr int BIT_COUNT = 28;

    DiscoveryFast() {
        discovered_.resize(2 << BIT_COUNT, 0);
    }

    bool Discover(const Board& b) {
        auto index = ((uint32_t)b.hash().to_ulong() << (32-BIT_COUNT)) >> (32-BIT_COUNT);
        if (discovered_[index] == 0 || discovered_[index] > b.CastCount()) {
            discovered_[index] = b.CastCount();
            return true;
        }
        return false;
    }

private:
    vector<uint8_t> discovered_;
};