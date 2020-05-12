#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <igraph.h>

#include "display.h"

igraph_integer_t compute_components(igraph_t* graph, igraph_vector_t* membership,
    igraph_vector_t* csize)
{
    igraph_integer_t vcount = igraph_vcount(graph);

    // Initialize components
    igraph_vector_init(membership, vcount);
    igraph_vector_init(csize, 1);

    // Compute components
    igraph_integer_t nb_clusters;
    igraph_clusters(graph, membership, csize, &nb_clusters, IGRAPH_WEAK);

    // Print the components
    fprintf(stderr, "Components: %d:", nb_clusters);
    for (igraph_integer_t i = 0; i < nb_clusters; ++i)
    {
        fprintf(stderr, " %d", (igraph_integer_t) VECTOR(*csize)[i]);
    }
    fprintf(stderr, "\nMembership:");
    for (igraph_integer_t i = 0; i < vcount; ++i)
    {
        fprintf(stderr, " %d", (igraph_integer_t) VECTOR(*membership)[i]);
    }
    fprintf(stderr, "\n");

    return nb_clusters;
}

void clean_graph(igraph_t* result, igraph_t* graph, igraph_vector_t* membership,
    igraph_integer_t component, igraph_integer_t component_size)
{
    igraph_integer_t vcount = igraph_vcount(graph);

    // Compute the look-up table
    igraph_vector_t lut;
    igraph_vector_init(&lut, vcount);
    igraph_integer_t current_index = 0;
    for (igraph_integer_t i = 0; i < vcount; ++i)
    {
        igraph_integer_t current = VECTOR(*membership)[i];
        if (current == component) {
            VECTOR(lut)[i] = current_index;
            current_index++;
        } else {
            VECTOR(lut)[i] = -1;
        }
    }
    fprintf(stderr, "LUT:");
    for (igraph_integer_t i = 0; i < vcount; ++i)
    {
        fprintf(stderr, " %d", (igraph_integer_t) VECTOR(lut)[i]);
    }
    fprintf(stderr, "\n");

    // Initialize the graph
    igraph_small(result, component_size, IGRAPH_UNDIRECTED, -1);

    // Initialize the selector
    igraph_es_t selector;
    igraph_es_all(&selector, IGRAPH_EDGEORDER_ID);

    // Initialize the iterator
    igraph_eit_t iterator;
    igraph_eit_create(graph, selector, &iterator);

    while (!IGRAPH_EIT_END(iterator))
    {
        igraph_integer_t from, to;
        igraph_edge(graph, IGRAPH_EIT_GET(iterator), &from, &to);

        igraph_integer_t component_from = VECTOR(*membership)[from];
        igraph_integer_t component_to = VECTOR(*membership)[to];

        if (component_from == component && component_to == component)
        {
            igraph_add_edge(result, VECTOR(lut)[from], VECTOR(lut)[to]);
        }

        IGRAPH_EIT_NEXT(iterator);
    }

    // Destroy the iterator
    igraph_eit_destroy(&iterator);

    // Destroy the selector
    igraph_es_destroy(&selector);

    // Destroy the lut
    igraph_vector_destroy(&lut);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [graph]\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input)
    {
        fprintf(stderr, "%s\n", strerror(errno));
    }

    igraph_t graph;
    // Create a new graph
    igraph_read_graph_edgelist(&graph, input, 0, false);

    // Display basic graph information
    graph_information(argv[1], &graph);

    // Compute the components
    igraph_vector_t membership;
    igraph_vector_t cluster_sizes;
    compute_components(&graph, &membership, &cluster_sizes);

    igraph_integer_t largest = igraph_vector_which_max(&cluster_sizes);

    // Compute the cluster graph
    igraph_t clean;
    clean_graph(&clean, &graph, &membership, largest,
        VECTOR(cluster_sizes)[largest]);

    // Simplify the cluster graph
    igraph_simplify(&clean, true, true, NULL);

    // Display basic graph information
    graph_information("clean", &clean);

    // Write it as dot format on stdout
    igraph_write_graph_edgelist(&clean, stdout);

    // Destroy the quotient graph
    igraph_destroy(&clean);

    // Destroy the components
    igraph_vector_destroy(&cluster_sizes);
    igraph_vector_destroy(&membership);

    // Destroy the graph
    igraph_destroy(&graph);

    fclose(input);

    return 0;
}
