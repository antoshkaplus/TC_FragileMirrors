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
# Note: -march=native may not be a good idea when running
#   on different machines the same executable
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

add_subdirectory(app/solver)
add_subdirectory(app/measure)