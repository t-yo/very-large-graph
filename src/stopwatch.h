#pragma once

#include  <time.h>

typedef struct stopwatch_point
{
    struct timespec timespec;
    clock_t clock;
} stopwatch_point_t;

typedef struct stopwatch
{
    double system_time;
    double cpu_time;
    double real_time;
} stopwatch_t;

/**
 * @brief Create a stopwatch point
 * @param stopwatch_point The stopwatch point struct to fill
 */
void create_stopwatch_point(stopwatch_point_t* stopwatch_point);

/**
 * @brief Initialize a stopwatch to zero
 * @param stopwatch The stopwatch to initialize
 */
void init_stopwatch(stopwatch_t* stopwatch);

/**
 * @brief Create a stopwatch
 * @param start The beginning stopwatch point
 * @param end The ending stopwatch point
 * @param stopwatch The stopwatch struct to fill
 */
void create_stopwatch(stopwatch_point_t* start,
        stopwatch_point_t* end,
        stopwatch_t* stopwatch);

/**
* @brief Increment a stopwatch
* @param start The beginning stopwatch point to increment
* @param end The ending stopwatch point to increment
* @param stopwatch The stopwatch struct to increment
*/
void increment_stopwatch(stopwatch_point_t* start,
        stopwatch_point_t* end,
        stopwatch_t* stopwatch);

/**
 * @brief Print a stopwatch
 * @param stopwatch The stopwatch to print
 */
void print_stopwatch(stopwatch_t* stopwatch);

/**
 * @brief Add stopwatches
 * @param lhs The left stopwatch
 * @param rhs The right stopwatch
 * @param result The result stopwatch
 */
void add_stopwatch(stopwatch_t* lhs,
        stopwatch_t* rhs,
        stopwatch_t* result);

/**
 * @brief Subtract stopwatches
 * @param lhs The left stopwatch
 * @param rhs The right stopwatch
 * @param result The result stopwatch
 */
void subtract_stopwatch(stopwatch_t* lhs,
        stopwatch_t* rhs,
        stopwatch_t* result);

/**
 * @brief Multiply stopwatches
 * @param lhs The left stopwatch
 * @param rhs The right stopwatch
 * @param result The result stopwatch
 */
void multiply_stopwatch(stopwatch_t* lhs,
        stopwatch_t* rhs,
        stopwatch_t* result);

/**
 * @brief Divide stopwatches
 * @param lhs The left stopwatch
 * @param rhs The right stopwatch
 * @param result The result stopwatch
 */
void divide_stopwatch(stopwatch_t* lhs,
        stopwatch_t* rhs,
        stopwatch_t* result);

/**
 * @brief Multiply stopwatches with a scalar
 * @param lhs The left stopwatch
 * @param scalar The scalar
 * @param result The result stopwatch
 */
void multiply_scalar_stopwatch(stopwatch_t* lhs,
        double scalar,
        stopwatch_t* result);
