cmake_minimum_required(VERSION 3.29)

include_directories(${lab_8_SOURCE_DIR}/static-lib/include)
add_library(static-lib STATIC static-lib.c)
