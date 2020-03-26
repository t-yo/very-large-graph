#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <igraph.h>

typedef struct options
{
    FILE* input;
    char* input_name;
} options_t;

void graph_information(char* name, igraph_t* graph)
{
    fprintf(stderr, "Name: %s\nVertices: %d\nEdges: %d\n", name,
        igraph_vcount(graph), igraph_ecount(graph));
}

bool parse_options(options_t* options, int argc, char** argv)
{
    options->input = stdin;
    options->input_name = "[stdin]";

    if (argc == 2)
    {
        options->input_name = argv[1];

        if (!(options->input = fopen(options->input_name, "r")))
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return false;
        }
    } else if (argc > 2)
    {
        fprintf(stderr, "Usage: %s [graph]\n", argv[0]);
        return false;
    }

    return true;
}

int main(int argc, char** argv)
{
    options_t options;
    if (!parse_options(&options, argc, argv))
        return 1;

    igraph_t graph;
    // Create a new graph
    igraph_read_graph_edgelist(&graph, options.input, 0, false);

    // Display basic graph information
    graph_information(options.input_name, &graph);

    // Write it as dot format on stdout
    igraph_write_graph_dot(&graph, stdout);;

    // Destroy the graph
    igraph_destroy(&graph);

    fclose(options.input);

    return 0;
}
