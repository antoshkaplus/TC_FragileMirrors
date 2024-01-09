

add_executable(meas_bs_restore_i3 bs_restore_i3.cpp
        ${CMAKE_SOURCE_DIR}/app/solver/main_template.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_n_i2.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(meas_bs_restore_i3 boost_program_options)

add_executable(meas_solv_1 solver_1.cpp
        ${CMAKE_SOURCE_DIR}/app/solver/main_template.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_r.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(meas_solv_1 boost_program_options)

add_executable(meas_solv_2 solver_2.cpp
        ${CMAKE_SOURCE_DIR}/app/solver/main_template.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_r.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(meas_solv_2 boost_program_options)
