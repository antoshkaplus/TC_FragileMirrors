#include "mirrors/board/board_r1.hpp"
#include "mirrors/board/board_r2.hpp"
#include "mirrors/board/board_r3.hpp"
#include "mirrors/board/board_r4.hpp"
#include "mirrors/board/board_r5.hpp"
#include "mirrors/board/board_r6.hpp"
#include "mirrors/board/board_r7.hpp"


std::vector<mirrors::cell_count_t> mirrors::Board_r::restore;
std::vector<mirrors::cell_count_t> mirrors::Board_r2::restore;

std::vector<mirrors::cell_count_t> mirrors::Board_r3::restore;
mirrors::board_size_t mirrors::Board_r3::size_;

uint32_t mirrors::Board_r4::destroyed_idx = 0;
mirrors::Grid<uint32_t> mirrors::Board_r4::destroyed;
std::vector<mirrors::Position> mirrors::Board_r4::destroyed_pos;

uint32_t mirrors::Board_r5::destroyed_idx = 0;
mirrors::Grid<uint32_t> mirrors::Board_r5::destroyed;
std::vector<mirrors::Position> mirrors::Board_r5::destroyed_pos;
std::vector<mirrors::cell_count_t> mirrors::Board_r5::destroyed_cast;

uint32_t mirrors::Board_r6::destroyed_idx = 0;
mirrors::Grid<uint32_t> mirrors::Board_r6::destroyed;
std::vector<mirrors::Position> mirrors::Board_r6::destroyed_pos;
std::vector<mirrors::cell_count_t> mirrors::Board_r6::destroyed_cast;

uint32_t mirrors::Board_r7::destroyed_idx = 0;
mirrors::Grid<uint32_t> mirrors::Board_r7::destroyed;
std::vector<mirrors::Position> mirrors::Board_r7::destroyed_pos;
std::vector<mirrors::cell_count_t> mirrors::Board_r7::destroyed_cast;