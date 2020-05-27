#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <igraph.h>

#include "display.h"
#include "vector.h"

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
    /*fprintf(stderr, "Components: %d: ", nb_clusters);
    vector_int_fprint(stderr, csize);
    fprintf(stderr, "\nMembership: ");
    vector_int_fprint(stderr, membership);
    fprintf(stderr, "\n");*/

    return nb_clusters;
}

void write_clean_graph(igraph_t* graph, igraph_vector_t* membership,
    igraph_integer_t component)
{
    igraph_integer_t vcount = igraph_vcount(graph);
    igraph_integer_t ecount = igraph_ecount(graph);

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
    /*fprintf(stderr, "LUT: ");
    vector_int_fprint(stderr, &lut);
    fprintf(stderr, "\n");*/

    // Initialize the selector
    igraph_es_t selector;
    igraph_es_all(&selector, IGRAPH_EDGEORDER_ID);

    // Initialize the iterator
    igraph_eit_t iterator;
    igraph_eit_create(graph, selector, &iterator);

    igraph_integer_t percentage = 0;
    fprintf(stderr,"Creating component graph: %d%%\n", percentage);
    igraph_integer_t e = 0;
    while (!IGRAPH_EIT_END(iterator))
    {
        igraph_integer_t from, to;
        igraph_edge(graph, IGRAPH_EIT_GET(iterator), &from, &to);

        igraph_integer_t component_from = VECTOR(*membership)[from];
        igraph_integer_t component_to = VECTOR(*membership)[to];

        if (component_from == component && component_to == component)
        {
            fprintf(stdout, "%d %d\n", (igraph_integer_t)VECTOR(lut)[from],
                (igraph_integer_t)VECTOR(lut)[to]);
        }

        e++;
        igraph_integer_t new_percentage = e * 100ll / ecount;
        if (new_percentage > percentage)
        {
            percentage = new_percentage;
            fprintf(stderr, "Creating component graph: %d%%\n",
                percentage);
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

    // Simplify the graph
    igraph_simplify(&graph, true, true, NULL);

    // Compute the components
    igraph_vector_t membership;
    igraph_vector_t cluster_sizes;
    compute_components(&graph, &membership, &cluster_sizes);

    igraph_integer_t largest = igraph_vector_which_max(&cluster_sizes);

    // Compute the cluster graph
    write_clean_graph(&graph, &membership, largest);

    // Destroy the components
    igraph_vector_destroy(&cluster_sizes);
    igraph_vector_destroy(&membership);

    // Destroy the graph
    igraph_destroy(&graph);

    fclose(input);

    return 0;
}
