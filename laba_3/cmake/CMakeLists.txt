cmake_minimum_required(VERSION 3.29)

project(Lab_03a)

set(CMAKE_C_COMPILER "clang")

add_executable(Lab-03a-server src/Lab-03a-server.c)
add_executable(Lab-03a-client src/Lab-03a-client.c)
add_executable(Lab-03b-server src/Lab-03b-server.c)
add_executable(Lab-03b-client src/Lab-03b-client.c)
