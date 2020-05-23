#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef struct options
{
    FILE* input;
    char* input_name;

    bool help;

    bool dot_original;
    bool dot_colored;
    bool dot_quotient;
    bool dot_weighted_quotient;

    bool print_membership;
    bool print_distances;
} options_t;

/**
 * @brief Parse the options
 * @param options The options
 * @param argc The number of arguments
 * @param argv The argument values
 * @return Whether the options were parsed successfully
 */
bool parse_options(int argc, char** argv, options_t* options);
