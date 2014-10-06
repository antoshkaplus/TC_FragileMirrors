<<<<<<< HEAD
# cmake -DCMAKE_BUILD_TYPE=Debug
# cmake -DCMAKE_BUILD_TYPE=Release


=======
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250

cmake_minimum_required (VERSION 2.8.11)
project (FRAGILE_MIRRORS)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

<<<<<<< HEAD
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -d -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall")
=======
>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250

file(GLOB SOURCE_FILES "./src/*.cpp")

include_directories("./include")

<<<<<<< HEAD
include_directories("/usr/local/include")
include_directories("/usr/include")

find_library(ANT_LIBRARY ant)

add_executable(FragileMirrors ${SOURCE_FILES})

target_link_libraries(FragileMirrors ${ANT_LIBRARY})
=======
add_executable(FragileMirrors ${SOURCE_FILES})

>>>>>>> f44c4f61d7d9b77a6c488bc54a6377b0f43f8250
