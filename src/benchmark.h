#pragma once

#include <time.h>
#include <stdio.h>

#include <igraph.h>

#define BENCHMARK(path, function)                                          \
    do {                                                                   \
        double total_elapsed = 0;                                          \
        double total_diameter = 0;                                         \
                                                                           \
        int tries = 0;                                                     \
                                                                           \
        while ((tries < 5) || (total_elapsed < 60))                        \
        {                                                                  \
            FILE* file = fopen((path), "r");                               \
            igraph_t graph;                                                \
            igraph_read_graph_edgelist(&graph, file, 0, false);            \
            fclose(file);                                                  \
                                                                           \
            struct timespec start;                                         \
            clock_gettime(CLOCK_REALTIME, &start);                         \
                                                                           \
            igraph_integer_t diameter = function(&graph);                  \
                                                                           \
            struct timespec end;                                           \
            clock_gettime(CLOCK_REALTIME, &end);                           \
                                                                           \
            igraph_destroy(&graph);                                        \
                                                                           \
            total_diameter += diameter;                                    \
                                                                           \
            double start_double = start.tv_nsec * 1E-9 + start.tv_sec;     \
            double end_double = end.tv_nsec * 1E-9 + end.tv_sec;           \
                                                                           \
            total_elapsed += end_double - start_double;                    \
                                                                           \
            tries += 1;                                                    \
        }                                                                  \
                                                                           \
        printf("Function: %s\n", "quotient_starting_double_sweep");        \
        printf("Tries: %d\n", tries);                                      \
        printf("Total elapsed: %f s\n", total_elapsed);                    \
        printf("Average elapsed: %f s\n", total_elapsed / tries);          \
        printf("Average time per try: %f try/s\n", tries / total_elapsed); \
        printf("Average diameter: %f\n", total_diameter / tries);          \
        printf("\n");                                                      \
    } while(false)
