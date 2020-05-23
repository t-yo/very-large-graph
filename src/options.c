#include "options.h"

#include <string.h>
#include <errno.h>

typedef int(* option_func_t)(int argc, char** argv, options_t* options);

typedef struct option
{
    char* option;
    option_func_t callback;
} option_t;

static int handle_dot_original(int argc, char** argv, options_t* options)
{
    (void) argc;
    (void) argv;
    options->dot_original = true;
    options->dot_colored = false;
    options->dot_quotient = false;
    options->dot_weighted_quotient = false;
    return 1;
}

static int handle_dot_quotient(int argc, char** argv, options_t* options)
{
    (void) argc;
    (void) argv;
    options->dot_original = false;
    options->dot_colored = false;
    options->dot_quotient = true;
    options->dot_weighted_quotient = false;
    return 1;
}

static int handle_dot_colored(int argc, char** argv, options_t* options)
{
    (void) argc;
    (void) argv;
    options->dot_original = false;
    options->dot_colored = true;
    options->dot_quotient = false;
    options->dot_weighted_quotient = false;
    return 1;
}

static int handle_dot_weighted_quotient(int argc, char** argv, options_t* options)
{
    (void) argc;
    (void) argv;
    options->dot_original = false;
    options->dot_colored = false;
    options->dot_quotient = false;
    options->dot_weighted_quotient = true;
    return 1;
}

static int handle_print_membership(int argc, char** argv, options_t* options)
{
    (void) argc;
    (void) argv;
    options->print_membership = true;
    return 1;
}

static int handle_print_distances(int argc, char** argv, options_t* options)
{
    (void) argc;
    (void) argv;
    options->print_distances = true;
    return 1;
}

static option_t all_options[] = {
    {
        .option = "--dot-original",
        .callback = handle_dot_original,
    },
    {
        .option = "--dot-colored",
        .callback = handle_dot_colored,
    },
    {
        .option = "--dot-quotient",
        .callback = handle_dot_quotient,
    },
    {
        .option = "--dot-weighted-quotient",
        .callback = handle_dot_weighted_quotient,
    },
    {
        .option = "--print-membership",
        .callback = handle_print_membership,
    },
    {
        .option = "--print-distances",
        .callback = handle_print_distances,
    },
};

bool parse_options(int argc, char** argv, options_t* options)
{
    options->input = stdin;
    options->input_name = "[stdin]";
    options->dot_original = false;
    options->dot_quotient = false;
    options->dot_colored = false;
    options->dot_weighted_quotient = false;
    options->print_membership = false;
    options->print_distances = false;

    int options_count = sizeof(all_options) / sizeof(option_t);
    int current_arg = 1;
    while (current_arg < argc)
    {
        int found = -1;
        for (int i = 0; i < options_count; ++i)
        {
            if (strcmp(all_options[i].option, argv[current_arg]) == 0)
            {
                found = all_options[i].callback(argc - current_arg,
                    argv + current_arg, options);
                break;
            }
        }
        if (found == -1)
        {
            break;
        }
        current_arg += found;
    }

    // If we don't have the filename at the end
    if (current_arg == argc)
    {
        return true;
    }

    // If we have the filename at the end
    if (current_arg + 1 == argc)
    {
        options->input_name = argv[current_arg];

        if (!(options->input = fopen(options->input_name, "r")))
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return false;
        }

        return true;
    }

    // If we have more arguments
    fprintf(stderr, "Usage: %s [options] [graph]\n", argv[0]);
    return false;
}