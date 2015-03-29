project(WizBall)
cmake_minimum_required(VERSION 2.6)
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} allegro allegro_image allegro_dialog allegro_ttf allegro_font allegro_primitives chipmunk m)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Ofast -Wall -pedantic")

add_executable("${PROJECT_NAME}_dbg" ${SRC_LIST})
target_link_libraries("${PROJECT_NAME}_dbg" allegro allegro_image allegro_dialog allegro_ttf allegro_font allegro_primitives chipmunk m)
set_target_properties("${PROJECT_NAME}_dbg" PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic")
