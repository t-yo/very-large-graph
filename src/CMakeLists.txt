add_executable(graph main.c)

find_package(IGRAPH REQUIRED)

target_include_directories(graph PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(graph PRIVATE ${IGRAPH_LIBRARIES})
target_compile_options(graph PRIVATE -Wall -Wextra -std=c11 -pedantic)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(graph PRIVATE -Werror -g3 -O0 -fsanitize=address)
    target_link_libraries(graph PRIVATE -fsanitize=address)
else()
    target_compile_options(graph PRIVATE -O3)
endif ()