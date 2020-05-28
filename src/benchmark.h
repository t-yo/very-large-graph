#pragma once

#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include <igraph.h>

#include "stopwatch.h"

#define BENCHMARK(path, function, min_tries, min_time)                       \
    do {                                                                     \
        stopwatch_t global_elapsed;                                          \
        stopwatch_t total_elapsed;                                           \
        init_stopwatch(&total_elapsed);                                      \
        double total_diameter = 0;                                           \
                                                                             \
        printf("--------------------------------------------------\n");    \
        printf("FUNCTION: %s\n\n", #function);                                 \
                                                                             \
        stopwatch_point_t global_start;                                      \
        create_stopwatch_point(&global_start);                               \
                                                                             \
        int tries = 0;                                                       \
                                                                             \
        while ((tries < min_tries) || (global_elapsed.real_time < min_time)) \
        {                                                                    \
            if (tries < min_tries)                                           \
            {                                                                \
                fprintf(stderr, "Read file ... ");                         \
            }                                                                \
                                                                             \
            FILE* file = fopen((path), "r");                                 \
            igraph_t graph;                                                  \
            igraph_read_graph_edgelist(&graph, file, 0, false);              \
            fclose(file);                                                    \
                                                                             \
            if (tries < min_tries)                                           \
            {                                                                \
                fprintf(stderr, "start try number %d ... ", tries);          \
            }                                                                \
                                                                             \
            stopwatch_point_t start_point;                                   \
            create_stopwatch_point(&start_point);                            \
                                                                             \
            igraph_integer_t diameter = function(&graph, tries < min_tries); \
                                                                             \
            stopwatch_point_t end_point;                                     \
            create_stopwatch_point(&end_point);                              \
                                                                             \
            if (tries < min_tries)                                           \
            {                                                                \
                fprintf(stderr, "diameter: %d\n", diameter);                 \
            }                                                                \
                                                                             \
            total_diameter += diameter;                                      \
            increment_stopwatch(&start_point, &end_point, &total_elapsed);   \
                                                                             \
            igraph_destroy(&graph);                                          \
                                                                             \
            tries += 1;                                                      \
                                                                             \
            stopwatch_point_t current_time;                                  \
            create_stopwatch_point(&current_time);                           \
            create_stopwatch(&global_start, &current_time, &global_elapsed); \
        }                                                                    \
                                                                             \
        stopwatch_t result;                                                  \
                                                                             \
        printf("\n- Tries:\t\t\t\t%d\n", tries);                             \
                                                                             \
        printf("- Total elapsed:\t\t\t");                                    \
        print_stopwatch(&global_elapsed);                                    \
                                                                             \
        printf("\n- Total elapsed (without file loading):\t");               \
        print_stopwatch(&total_elapsed);                                     \
                                                                             \
        printf("\n- Loading time:\t\t\t\t%fs\n",                             \
            global_elapsed.real_time - total_elapsed.real_time);             \
                                                                             \
        printf("- Average time per try:\t\t\t");                             \
        multiply_scalar_stopwatch(&total_elapsed, 1.0 / tries, &result);     \
        print_stopwatch(&result);                                            \
                                                                             \
        printf("\n- Average try per second:\t\t%f try/s\n",                  \
            tries / total_elapsed.real_time);                                \
        printf("- Average diameter:\t\t\t%f\n\n\n", total_diameter / tries); \
    } while(false)
