cmake_minimum_required(VERSION 3.28)

# Название главного проекта
project(lab_8)

# Указываем компилятор Clang
set(CMAKE_C_COMPILER "clang")

# Настраиваем флаги линкера для подсистемы консоли
# set(CMAKE_EXE_LINKER_FLAGS "-Xlinker /subsystem:console")

# Устанавливаем пути для вывода
# set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
# set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

# Добавляем поддиректории
add_subdirectory(main-project)
add_subdirectory(dynamic-lib)


SET_TARGET_PROPERTIES(dynamic-lib PROPERTIES
   LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
)
SET_TARGET_PROPERTIES(main-project PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
)