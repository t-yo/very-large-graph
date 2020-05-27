#pragma once

#include <time.h>
#include <stdio.h>

#include <igraph.h>

#include "stopwatch.h"

#define BENCHMARK(path, function)                                            \
    do {                                                                     \
        stopwatch_t global_elapsed;                                          \
        stopwatch_t total_elapsed;                                           \
        init_stopwatch(&total_elapsed);                                      \
        double total_diameter = 0;                                           \
                                                                             \
        stopwatch_point_t global_start;                                      \
        create_stopwatch_point(&global_start);                               \
                                                                             \
        int tries = 0;                                                       \
        FILE* file = fopen((path), "r");                                     \
        igraph_t graph;                                                      \
        igraph_read_graph_edgelist(&graph, file, 0, false);                  \
        fclose(file);                                                        \
                                                                             \
        while ((tries < 3) || (global_elapsed.real_time < 60))                \
        {                                                                    \
            stopwatch_point_t start_point;                                   \
            create_stopwatch_point(&start_point);                            \
                                                                             \
            igraph_integer_t diameter = function(&graph);                    \
                                                                             \
            stopwatch_point_t end_point;                                     \
            create_stopwatch_point(&end_point);                              \
                                                                             \
            total_diameter += diameter;                                      \
            increment_stopwatch(&start_point, &end_point, &total_elapsed);   \
                                                                             \
            stopwatch_point_t current_time;                                  \
            create_stopwatch_point(&current_time);                           \
            create_stopwatch(&global_start, &current_time, &global_elapsed); \
                                                                             \
            tries += 1;                                                      \
        }                                                                    \
                                                                             \
        igraph_destroy(&graph);                                              \
                                                                             \
        stopwatch_t result;                                                  \
                                                                             \
                                                                             \
        printf("Function:\t\t\t\t%s\n", #function);                          \
                                                                             \
        printf("Tries:\t\t\t\t\t%d\n", tries);                               \
                                                                             \
        printf("Total elapsed:\t\t\t\t");                                    \
        print_stopwatch(&global_elapsed);                                    \
                                                                             \
        printf("\nTotal elapsed (without file loading):\t");                 \
        print_stopwatch(&total_elapsed);                                     \
                                                                             \
        printf("\nLoading time:\t\t\t\t%fs\n",                               \
            global_elapsed.real_time - total_elapsed.real_time);             \
                                                                             \
        printf("Average time per try:\t\t\t");                               \
        multiply_scalar_stopwatch(&total_elapsed, 1.0 / tries, &result);     \
        print_stopwatch(&result);                                            \
                                                                             \
        printf("\nAverage try per second:\t\t\t%f try/s\n",                  \
            tries / total_elapsed.real_time);                                \
        printf("Average diameter:\t\t\t%f\n", total_diameter / tries);       \
        printf("\n");                                                        \
    } while(false)
