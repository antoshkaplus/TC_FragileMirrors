
add_executable(solv_greedy_i1 main_template.cpp greedy_i1.cpp)
target_link_libraries(solv_greedy_i1 boost_program_options)

add_executable(solv_bs_greedy_i1 main_template.cpp bs_greedy_i1.cpp ${RNG_FILE})
target_link_libraries(solv_bs_greedy_i1 boost_program_options)

add_executable(solv_bs_score_i1 main_template.cpp bs_score_i1.cpp ${RNG_FILE})
target_link_libraries(solv_bs_score_i1 boost_program_options)

add_executable(solv_bs_restore_i1 main_template.cpp bs_restore_i1.cpp ${RNG_FILE} ${CMAKE_SOURCE_DIR}/src/mirrors/board/cast_i1.cpp)
target_link_libraries(solv_bs_restore_i1 boost_program_options)

set(OTHERS_DIR ${CMAKE_SOURCE_DIR}/others)

# solution from other people
# need fragmir for helper functions
add_executable(colun main_template.cpp ${OTHERS_DIR}/colun.cpp)
target_link_libraries(colun boost_program_options)

add_executable(chokudai main_template.cpp ${OTHERS_DIR}/chokudai.cpp)
target_link_libraries(chokudai fragmir)

add_executable(psyho main_template.cpp ${OTHERS_DIR}/psyho.cpp)
target_link_libraries(psyho fragmir)

add_executable(eldidou main_template.cpp ${OTHERS_DIR}/eldidou.cpp)
target_link_libraries(eldidou boost_program_options)