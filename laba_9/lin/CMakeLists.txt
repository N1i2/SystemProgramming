cmake_minimum_required(VERSION 3.28)

project(laba_9)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "clang")

add_executable(service09d src/service09d.c src/become_daemon.c)