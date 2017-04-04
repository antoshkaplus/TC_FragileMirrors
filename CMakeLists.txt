# cmake -DCMAKE_BUILD_TYPE=Debug
# cmake -DCMAKE_BUILD_TYPE=Release
# make

cmake_minimum_required (VERSION 3.2)
project (FragileMirrors)

list(APPEND CMAKE_CXX_FLAGS " -std=c++1z -Werror -Wall ")

set(CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG} ")
set(CMAKE_CXX_FLAGS_RELEASE " ${CMAKE_CXX_FLAGS_RELEASE} -O3 ")


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

include_directories("./include")
include_directories("/usr/local/include")
include_directories("/usr/include")

find_library(ANT_LIBRARY ant)

add_library(fragmir ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(fragmir ${ANT_LIBRARY})

add_executable(solver "app/main_template.cpp" "app/solver.cpp")
target_link_libraries(solver fragmir)

# quick run to see result of the change
add_executable(tester "app/tester.cpp")
target_link_libraries(tester fragmir)

# solution from other people
# need fragmir for helper functions
add_executable(colun "app/main_template.cpp" "others/colun.cpp")
target_link_libraries(colun fragmir)

add_executable(chokudai "app/main_template.cpp" "others/chokudai.cpp")
target_link_libraries(chokudai fragmir)

add_executable(psyho "app/main_template.cpp" "others/psyho.cpp")
target_link_libraries(psyho fragmir)

add_executable(eldidou "app/main_template.cpp" "others/eldidou.cpp")
target_link_libraries(eldidou fragmir)

# tests
file(GLOB_RECURSE TEST_SOURCE_FILES "test/*.cpp")

add_executable(unittest ${TEST_SOURCE_FILES} test/board.cpp)
target_link_libraries(unittest fragmir gtest)