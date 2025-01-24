cmake_minimum_required(VERSION 3.29)

project(lab_8)

set(CMAKE_C_COMPILER "clang")

# Добавляем поддиректории для статической библиотеки и основного проекта
add_subdirectory(main-project)
add_subdirectory(static-lib)
