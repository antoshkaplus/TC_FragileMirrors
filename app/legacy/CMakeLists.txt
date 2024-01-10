# Does not work as is. Need to debug building.

find_library(ANT_LIBRARY ant)
if (NOT ANT_LIBRARY)
    message(FATAL_ERROR "ant library not found")
endif()

add_library(fragmir ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(fragmir ${ANT_LIBRARY})

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

# depth - calculate stats to run beam search with time limits
add_executable(depth app/bs_best_width.cpp)
target_link_libraries(depth fragmir tbb)
