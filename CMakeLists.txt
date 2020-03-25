cmake_minimum_required(VERSION 3.15)
project(graph C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH  "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(src)