#pragma once

#include <igraph_datatype.h>

/**
 * @brief Compute the quotient graph using a clustering
 * @param graph The input graph
 * @param nb_clusters The number of clusters
 * @param membership The membership vector
 * @param result The quotient graph
 */
void quotient_graph(igraph_t* graph, igraph_integer_t nb_clusters,
    igraph_vector_t* membership, igraph_t* result);
