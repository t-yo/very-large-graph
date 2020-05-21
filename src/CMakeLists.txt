add_library(lib
    display.c display.h
    color.c color.h
    quotient.c quotient.h
    sweep.c sweep.h
    vector.c vector.h)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c11")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast -march=native")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Werror -O0 -g3 -fsanitize=address")

add_executable(graph main.c)
add_executable(extract extract.c)

find_package(IGRAPH REQUIRED)

target_include_directories(lib PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(lib PRIVATE ${IGRAPH_LIBRARIES} m)

target_include_directories(graph PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(graph PRIVATE ${IGRAPH_LIBRARIES} lib)

target_include_directories(extract PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(extract PRIVATE ${IGRAPH_LIBRARIES} lib)
