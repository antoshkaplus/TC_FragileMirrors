#pragma once
#include <stdexcept>
#include <cstdlib>


namespace mirrors {

using board_size_t = int8_t;
using cell_count_t = int16_t;
using hash_value_t = uint64_t;

enum class Direction : int8_t {
    None,
    Up,
    Down,
    Right,
    Left,
};

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

constexpr char kChRightMirror = '\\';
constexpr char kChLeftMirror = '/';

inline Mirror ConvertChMirrorToMirror(char mirror) {
    if (mirror == kChRightMirror) return Mirror::Right;
    if (mirror == kChLeftMirror) return Mirror::Left;
    throw std::runtime_error("Unexpected char mirror.");
}
inline Mirror ConvertLetterMirrorToMirror(char mirror) {
    if (mirror == kLetterRightMirror) return Mirror::Right;
    if (mirror == kLetterLeftMirror) return Mirror::Left;
    throw std::runtime_error("Unexpected char mirror.");
}

}