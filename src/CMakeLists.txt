add_library(lib display.c display.h color.c color.h quotient.c quotient.h sweep.c sweep.h vector.c vector.h)

add_executable(graph main.c)
add_executable(extract extract.c)

find_package(IGRAPH REQUIRED)

target_include_directories(lib PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(lib PRIVATE ${IGRAPH_LIBRARIES} m)
target_compile_options(lib PRIVATE -Wall -Wextra -std=c11 -pedantic)

target_include_directories(graph PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(graph PRIVATE ${IGRAPH_LIBRARIES} lib)
target_compile_options(graph PRIVATE -Wall -Wextra -std=c11 -pedantic)

target_include_directories(extract PRIVATE ${IGRAPH_INCLUDES})
target_link_libraries(extract PRIVATE ${IGRAPH_LIBRARIES} lib)
target_compile_options(extract PRIVATE -Wall -Wextra -std=c11 -pedantic)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(lib PRIVATE -Werror -g3 -O0 -fsanitize=address)
    target_link_libraries(lib PRIVATE -fsanitize=address)

    target_compile_options(graph PRIVATE -Werror -g3 -O0 -fsanitize=address)
    target_link_libraries(graph PRIVATE -fsanitize=address)

    target_compile_options(extract PRIVATE -Werror -g3 -O0 -fsanitize=address)
    target_link_libraries(extract PRIVATE -fsanitize=address)
else()
    target_compile_options(lib PRIVATE -O3)

    target_compile_options(graph PRIVATE -O3)

    target_compile_options(extract PRIVATE -O3)
endif ()
