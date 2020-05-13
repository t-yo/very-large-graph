#include "quotient.h"

#include <igraph.h>

void quotient_graph(igraph_t* graph, igraph_integer_t nb_clusters,
    igraph_vector_t* membership, igraph_t* result)
{
    // Initialize the graph
    igraph_small(result, nb_clusters, IGRAPH_UNDIRECTED, -1);

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

        igraph_integer_t quotient_from = VECTOR(*membership)[from];
        igraph_integer_t quotient_to = VECTOR(*membership)[to];

        if (quotient_from != quotient_to)
        {
            igraph_bool_t connected;
            igraph_are_connected(result, quotient_from, quotient_to, &connected);
            if (!connected)
            {
                igraph_add_edge(result, quotient_from, quotient_to);
            }
        }

        IGRAPH_EIT_NEXT(iterator);
    }

    // Destroy the iterator
    igraph_eit_destroy(&iterator);

    // Destroy the selector
    igraph_es_destroy(&selector);
}
