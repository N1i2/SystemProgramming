cmake_minimum_required(VERSION 3.29)

include_directories(${lab_8_SOURCE_DIR}/dynamic-lib/include)
add_library(dynamic-lib SHARED dynamic-lib.c)
