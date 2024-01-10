
add_executable(solv_greedy_i1 main_template.cpp greedy_i1.cpp)
target_link_libraries(solv_greedy_i1 boost_program_options)

add_executable(solv_bs_greedy_i1 main_template.cpp bs_greedy_i1.cpp ${RNG_FILE})
target_link_libraries(solv_bs_greedy_i1 boost_program_options)

add_executable(solv_bs_score_i1 main_template.cpp bs_score_i1.cpp ${RNG_FILE})
target_link_libraries(solv_bs_score_i1 boost_program_options)

add_executable(solv_bs_score_i2 main_template.cpp bs_score_i2.cpp ${RNG_FILE})
target_link_libraries(solv_bs_score_i2 boost_program_options)

add_executable(solv_bs_restore_i1 main_template.cpp bs_restore_i1.cpp ${RNG_FILE} ${CMAKE_SOURCE_DIR}/src/mirrors/board/cast_i1.cpp)
target_link_libraries(solv_bs_restore_i1 boost_program_options)

add_executable(solv_bs_restore_i3 main_template.cpp bs_restore_i3.cpp
               ${RNG_FILE}
               ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_n_i2.cpp
               ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_r.cpp
               ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(solv_bs_restore_i3 boost_program_options)

add_executable(solv_bs_restore_i4 main_template.cpp bs_restore_i4.cpp ${RNG_FILE} ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_n_i4.cpp)
target_link_libraries(solv_bs_restore_i4 boost_program_options)

add_executable(solv_bs_restore_i5 main_template.cpp bs_restore_i5.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_n_i2.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(solv_bs_restore_i5 boost_program_options)

add_executable(solv_bs_restore_i6 main_template.cpp bs_restore_i6.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_n_i2.cpp
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_r.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(solv_bs_restore_i6 boost_program_options)

add_executable(solv_bs_restore_i7 main_template.cpp bs_restore_i7.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_n_i2.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(solv_bs_restore_i7 boost_program_options)

add_executable(solv_bs_restore_i8 main_template.cpp bs_restore_i8.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_n_i5.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(solv_bs_restore_i8 boost_program_options)

add_executable(solv_1 main_template.cpp solver_1.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_r.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(solv_1 boost_program_options)

add_executable(solv_2 main_template.cpp solver_2.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_r.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(solv_2 boost_program_options)

add_executable(solv_3 main_template.cpp solver_3.cpp
        ${RNG_FILE}
        ${CMAKE_SOURCE_DIR}/src/mirrors/board/board_r.cpp
        ${CMAKE_SOURCE_DIR}/src/legacy/score_.cpp)
target_link_libraries(solv_3 boost_program_options)


set(OTHERS_DIR ${CMAKE_SOURCE_DIR}/others)

# solution from other people
add_executable(colun main_template.cpp ${OTHERS_DIR}/colun.cpp)
target_link_libraries(colun boost_program_options)

add_executable(chokudai main_template.cpp ${OTHERS_DIR}/chokudai.cpp)
target_link_libraries(chokudai boost_program_options)

add_executable(psyho main_template.cpp ${OTHERS_DIR}/psyho.cpp)
target_link_libraries(psyho boost_program_options)

add_executable(eldidou main_template.cpp ${OTHERS_DIR}/eldidou.cpp)
target_link_libraries(eldidou boost_program_options)