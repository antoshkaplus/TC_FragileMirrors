#pragma once
#include <random>
#include <stdexcept>
#include <cstdlib>
#include <iostream>


namespace mirrors {

using board_size_t = int8_t;
using cell_count_t = int16_t;
using deriv_count_t = int32_t;
using hash_value_t = uint64_t;
using dir_t = int8_t;
using mir_t = int8_t;


enum class Direction : int8_t {
    None,
    Up,
    Down,
    Right,
    Left,
};

// When implementing board with neighbour links array.
const constexpr dir_t kDirTop      = 0;
const constexpr dir_t kDirBottom   = 1;
const constexpr dir_t kDirLeft     = 2;
const constexpr dir_t kDirRight    = 3;

const constexpr std::array<dir_t, 4> kDirOpposite = { {
      kDirBottom,
      kDirTop,
      kDirRight,
      kDirLeft,
} };

const constexpr mir_t kMirLeft  = 0;
const constexpr dir_t kMirRight = 1;

const constexpr std::array<std::array<dir_t, 4>, 2> kMirDirections = {{
    // from direction to direction
    // kMirLeft
    {kDirLeft, kDirRight, kDirTop, kDirBottom},
    // kMirRight
    {kDirRight, kDirLeft, kDirBottom, kDirTop}
}};


enum class Mirror : int8_t {
    None,
    Left,
    Right,
    Border,
    Destroyed,
};

inline std::ostream& operator<<(std::ostream& out, Mirror mirror) {
    switch (mirror) {
        case Mirror::None:
            out << "None";
            break;
        case Mirror::Left:
            out << "Left";
            break;
        case Mirror::Right:
            out << "Right";
            break;
        case Mirror::Border:
            out << "Border";
            break;
        case Mirror::Destroyed:
            out << "Destroyed";
            break;
        default:
            out << "Unknown";
    }
    return out;
}

constexpr char kLetterRightMirror = 'R';
constexpr char kLetterLeftMirror = 'L';
constexpr char kLetterDestroyedMirror = 'D';

constexpr char kChRightMirror = '\\';
constexpr char kChLeftMirror = '/';

inline Mirror ConvertChMirrorToMirror(char mirror) {
    if (mirror == kChRightMirror) return Mirror::Right;
    if (mirror == kChLeftMirror) return Mirror::Left;
    throw std::runtime_error("Unexpected char mirror.");
}

inline mir_t ConvertChMirrorToMir(char mirror) {
    if (mirror == kChRightMirror) return kMirRight;
    if (mirror == kChLeftMirror) return kMirLeft;
    throw std::runtime_error("Unexpected char mirror.");
}

inline Mirror ConvertLetterMirrorToMirror(char mirror) {
    if (mirror == kLetterRightMirror) return Mirror::Right;
    if (mirror == kLetterLeftMirror) return Mirror::Left;
    if (mirror == kLetterDestroyedMirror) return Mirror::Destroyed;
    throw std::runtime_error("Unexpected char mirror.");
}

inline mir_t ConvertLetterMirrorToMir(char mirror) {
    if (mirror == kLetterRightMirror) return kMirRight;
    if (mirror == kLetterLeftMirror) return kMirLeft;
    throw std::runtime_error("Unexpected char mirror.");
}

inline char ConvertMirrorToLetterMirror(Mirror mirror) {
    if (mirror == Mirror::Right) return kLetterRightMirror;
    if (mirror == Mirror::Left) return kLetterLeftMirror;
    if (mirror == Mirror::Destroyed) return kLetterDestroyedMirror;
    throw std::runtime_error("Unexpected char mirror.");
}

}