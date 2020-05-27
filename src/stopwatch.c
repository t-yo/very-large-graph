#include "stopwatch.h"

#include <stdio.h>

void create_stopwatch_point(stopwatch_point_t* stopwatch_point)
{
    clock_gettime(CLOCK_REALTIME, &(stopwatch_point->timespec));
    stopwatch_point->clock = clock();
}

void init_stopwatch(stopwatch_t* stopwatch)
{
    stopwatch->real_time = 0;
    stopwatch->cpu_time = 0;
    stopwatch->system_time = 0;
}

void create_stopwatch(stopwatch_point_t* start,
        stopwatch_point_t* end,
        stopwatch_t* stopwatch)
{
    // Compute real time
    double real_time_start =
            start->timespec.tv_nsec * 1E-9 + start->timespec.tv_sec;
    double real_time_end =
            end->timespec.tv_nsec * 1E-9 + end->timespec.tv_sec;
    stopwatch->real_time = real_time_end - real_time_start;

    // Compute cpu time
    stopwatch->cpu_time =
            ((double) (end->clock - start->clock)) / CLOCKS_PER_SEC;

    // Compute user time
    stopwatch->system_time = stopwatch->real_time - stopwatch->cpu_time;
}

void increment_stopwatch(stopwatch_point_t* start,
        stopwatch_point_t* end,
        stopwatch_t* stopwatch)
{
    // Compute real time
    double real_time_start =
            start->timespec.tv_nsec * 1E-9 + start->timespec.tv_sec;
    double real_time_end =
            end->timespec.tv_nsec * 1E-9 + end->timespec.tv_sec;
    stopwatch->real_time += real_time_end - real_time_start;

    // Compute cpu time
    stopwatch->cpu_time +=
            ((double) (end->clock - start->clock)) / CLOCKS_PER_SEC;

    // Compute user time
    stopwatch->system_time += stopwatch->real_time - stopwatch->cpu_time;
}

void print_stopwatch(stopwatch_t* stopwatch)
{
    printf("(cpu: %fs | sys: %fs | real: %fs)",
            stopwatch->cpu_time,
            stopwatch->system_time,
            stopwatch->real_time);
}

void add_stopwatch(stopwatch_t* lhs,
        stopwatch_t* rhs,
        stopwatch_t* result)
{
    result->system_time = lhs->system_time + rhs->system_time;
    result->cpu_time = lhs->cpu_time + rhs->cpu_time;
    result->real_time = lhs->real_time + rhs->real_time;
}

void subtract_stopwatch(stopwatch_t* lhs,
        stopwatch_t* rhs,
        stopwatch_t* result)
{
    result->system_time = lhs->system_time - rhs->system_time;
    result->cpu_time = lhs->cpu_time - rhs->cpu_time;
    result->real_time = lhs->real_time - rhs->real_time;
}

void multiply_stopwatch(stopwatch_t* lhs,
        stopwatch_t* rhs,
        stopwatch_t* result)
{
    result->system_time = lhs->system_time * rhs->system_time;
    result->cpu_time = lhs->cpu_time * rhs->cpu_time;
    result->real_time = lhs->real_time * rhs->real_time;
}

void divide_stopwatch(stopwatch_t* lhs,
        stopwatch_t* rhs,
        stopwatch_t* result)
{
    result->system_time = lhs->system_time / rhs->system_time;
    result->cpu_time = lhs->cpu_time / rhs->cpu_time;
    result->real_time = lhs->real_time / rhs->real_time;
}

void multiply_scalar_stopwatch(stopwatch_t* lhs,
        double scalar,
        stopwatch_t* result)
{
    result->system_time = lhs->system_time * scalar;
    result->cpu_time = lhs->cpu_time * scalar;
    result->real_time = lhs->real_time * scalar;
}