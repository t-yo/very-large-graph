cmake_minimum_required(VERSION 3.15)
project(graph C)

set(CMAKE_C_STANDARD 11)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

add_library(lib
        src/display.c
        src/display.h
        src/color.c
        src/color.h
        src/quotient.c
        src/quotient.h
        src/sweep.c
        src/sweep.h
        src/vector.c
        src/vector.h
        src/matrix.c
        src/matrix.h
        src/options.c
        src/options.h
        src/communities.c
        src/communities.h
        src/stopwatch.c
        src/stopwatch.h)

set(CMAKE_MODULE_PATH  "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c11")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast -march=native")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Werror -O0 -g3 -fsanitize=address")

add_executable(graph src/main.c)
add_executable(extract src/extract.c)
add_executable(benchmark src/benchmark.c src/benchmark.h)

find_package(IGRAPH REQUIRED)

target_include_directories(lib PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(lib PRIVATE ${IGRAPH_LIBRARIES} m)

target_include_directories(graph PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(graph PRIVATE ${IGRAPH_LIBRARIES} lib)

target_include_directories(extract PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(extract PRIVATE ${IGRAPH_LIBRARIES} lib)

target_include_directories(benchmark PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(benchmark PRIVATE ${IGRAPH_LIBRARIES} lib)