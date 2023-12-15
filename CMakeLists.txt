# cmake -DCMAKE_BUILD_TYPE=Debug
# cmake -DCMAKE_BUILD_TYPE=Release
# make

cmake_minimum_required (VERSION 3.2)
project (FragileMirrors)

string(APPEND CMAKE_CXX_FLAGS " -std=c++23 -Werror -Wall -Wno-narrowing -Wno-parentheses -Wno-char-subscripts"
                            " -Wno-unused-const-variable -Wno-unused-but-set-variable -Wno-sign-compare"
                            " -Wno-delete-non-virtual-dtor")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    string(APPEND CMAKE_CXX_FLAGS -stdlib=libc++)
endif()

set(CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG} ")
# set before adding more options to RELEASE
set(CMAKE_CXX_FLAGS_FAST " ${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native")
set(CMAKE_CXX_FLAGS_RELEASE " ${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
set(CMAKE_CXX_FLAGS_PROFILE " ${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -pg")


set(BinDir ${PROJECT_SOURCE_DIR}/bin)
set(LibDir ${PROJECT_SOURCE_DIR}/lib)

# archive is for static libraries
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinDir} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LibDir} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LibDir} )
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BinDir} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LibDir} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LibDir} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )


file(GLOB SOURCE_FILES "./src/*.cpp")
file(GLOB HEADER_FILES "./include/*.h" "./include/*.hpp")
set(RNG_FILE ${CMAKE_SOURCE_DIR}/src/mirrors/common/rng.cpp)

include_directories("./include")
include_directories("/usr/local/include")
include_directories("/usr/include")

find_library(ANT_LIBRARY ant)
if (NOT ANT_LIBRARY)
    message(FATAL_ERROR "ant library not found")
endif()

add_library(fragmir ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(fragmir ${ANT_LIBRARY})

add_subdirectory(app/solver)
add_subdirectory(app/measure)

add_executable(solver "app/solver/main_template.cpp" "app/solver.cpp")
target_link_libraries(solver fragmir ant)

# quick run to see result of the change
add_executable(tester "app/tester.cpp")
target_link_libraries(tester fragmir)

add_executable(bs_best_width app/bs_best_width.cpp)
target_link_libraries(bs_best_width fragmir)

add_executable(bs_new app/solver/main_template.cpp app/bs_new.cpp)
target_link_libraries(bs_new fragmir)

add_executable(bs app/solver/main_template.cpp app/bs.cpp)
target_link_libraries(bs fragmir)

add_executable(bs_balanced "app/solver/main_template.cpp" "app/bs_balanced.cpp")
target_link_libraries(bs_balanced fragmir)



find_library(GTEST_LIBRARY gtest)
if (NOT GTEST_LIBRARY)
    message(FATAL_ERROR "gtest library not found")
endif()

# tests
file(GLOB_RECURSE TEST_SOURCE_FILES "test/*.cpp")

add_executable(unittest ${TEST_SOURCE_FILES})
target_link_libraries(unittest fragmir ${GTEST_LIBRARY})


find_library(GBENCHMARK_LIBRARY benchmark)
if (NOT GBENCHMARK_LIBRARY)
    message(FATAL_ERROR "benchmark library not found")
endif()

# benchmarks
file(GLOB_RECURSE BENCHMARK_SOURCE_FILES "benchmark/*.cpp")

add_executable(benchmark ${BENCHMARK_SOURCE_FILES})
target_link_libraries(benchmark fragmir ${GBENCHMARK_LIBRARY})
if(UNIX AND NOT APPLE)
    target_link_libraries(benchmark pthread)
endif()

# depth - calculate stats to run beam search with time limits
add_executable(depth app/bs_best_width.cpp)
target_link_libraries(depth fragmir tbb)