
cmake_minimum_required (VERSION 2.8.11)
project (FRAGILE_MIRRORS)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")


file(GLOB SOURCE_FILES "./src/*.cpp")

include_directories("./include")

add_executable(FragileMirrors ${SOURCE_FILES})

