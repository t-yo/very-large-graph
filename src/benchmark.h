#pragma once

#include <time.h>
#include <stdio.h>

#include <igraph.h>

#define BENCHMARK(path, function)                                          \
    do {                                                                   \
        double global_elapsed = 0;                                         \
        double total_elapsed = 0;                                          \
        double total_diameter = 0;                                         \
                                                                           \
        struct timespec global_start;                                      \
        clock_gettime(CLOCK_REALTIME, &global_start);                      \
        double global_start_double =                                       \
            global_start.tv_nsec * 1E-9 + global_start.tv_sec;             \
                                                                           \
        int tries = 0;                                                     \
        FILE* file = fopen((path), "r");                                   \
        igraph_t graph;                                                    \
        igraph_read_graph_edgelist(&graph, file, 0, false);                \
        fclose(file);                                                      \
                                                                           \
        while ((tries < 3) || (global_elapsed < 60))                       \
        {                                                                  \
            struct timespec start;                                         \
            clock_gettime(CLOCK_REALTIME, &start);                         \
                                                                           \
            igraph_integer_t diameter = function(&graph);                  \
                                                                           \
            struct timespec end;                                           \
            clock_gettime(CLOCK_REALTIME, &end);                           \
                                                                           \
                                                                           \
            total_diameter += diameter;                                    \
                                                                           \
            double start_double = start.tv_nsec * 1E-9 + start.tv_sec;     \
            double end_double = end.tv_nsec * 1E-9 + end.tv_sec;           \
                                                                           \
            total_elapsed += end_double - start_double;                    \
                                                                           \
            struct timespec current_time;                                  \
            clock_gettime(CLOCK_REALTIME, &current_time);                  \
            double current_time_double =                                   \
                current_time.tv_nsec * 1E-9 + current_time.tv_sec;         \
                                                                           \
            global_elapsed = current_time_double - global_start_double;    \
                                                                           \
            tries += 1;                                                    \
        }                                                                  \
                                                                           \
        igraph_destroy(&graph);                                            \
                                                                           \
        printf("Function: %s\n", #function);                               \
        printf("Tries: %d\n", tries);                                      \
        printf("Total elapsed: %f s\n", total_elapsed);                    \
        printf("Average elapsed: %f s\n", total_elapsed / tries);          \
        printf("Average time per try: %f try/s\n", tries / total_elapsed); \
        printf("Average diameter: %f\n", total_diameter / tries);          \
        printf("\n");                                                      \
    } while(false)
