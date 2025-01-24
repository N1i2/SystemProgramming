cmake_minimum_required(VERSION 3.29)

include_directories(${lab_8_SOURCE_DIR}/static-lib/include)
link_directories(${lab_8_SOURCE_DIR}/static-lib/src)
add_executable(main-project main-project.c)
target_link_libraries(main-project static-lib)