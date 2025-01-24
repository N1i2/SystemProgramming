cmake_minimum_required(VERSION 3.28)

# Название главного проекта
project(lab_8)

# Указываем компилятор Clang
set(CMAKE_C_COMPILER "clang")

# Добавляем поддиректории для статической библиотеки и основного проекта
add_subdirectory(main-project)
add_subdirectory(static-lib)
