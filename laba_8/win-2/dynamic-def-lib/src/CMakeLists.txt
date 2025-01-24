cmake_minimum_required(VERSION 3.29)

include_directories(${lab_8_SOURCE_DIR}/dynamic-def-lib/include)

# Создание библиотеки
add_library(dynamic-def-lib SHARED dynamic-def-lib.c)

# Указываем путь к .def файлу
set(DEF_FILE "${lab_8_SOURCE_DIR}/dynamic-def-lib/src/dynamic-def-lib.def")

# Линкуем .def файл с использованием флага /DEF
set_target_properties(dynamic-def-lib PROPERTIES LINK_FLAGS "-Xlinker /DEF:${DEF_FILE}")