cmake_minimum_required(VERSION 3.29)

# Включение заголовочных файлов библиотеки
include_directories(${lab_8_SOURCE_DIR}/dynamic-lib/include)
link_directories(${lab_8_SOURCE_DIR}/dynamic-lib/src)

# Включение заголовочных файлов библиотеки
include_directories(${lab_8_SOURCE_DIR}/dynamic-def-lib/include)
link_directories(${lab_8_SOURCE_DIR}/dynamic-def-lib/src)

# Создание исполняемого файла
add_executable(main-project main-project.c)

# Добавление библиотеки
target_link_libraries(main-project dynamic-lib)
target_link_libraries(main-project dynamic-def-lib)