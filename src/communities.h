#pragma once

#include <igraph_datatype.h>

/**
 * @brief Compute the communities using Louvain
 * @param graph The graph
 * @param membership The membership of each vertex (out)
 * @return The number of clusters
 */
igraph_integer_t compute_communities_louvain(igraph_t* graph,
    igraph_vector_t* membership);

/**
 * @brief Compute the communities using Leiden
 * @param graph The graph
 * @param resolution The resolution for Leiden
 * @param beta The beta for Leiden
 * @param membership The membership of each vertex (out)
 * @return The number of clusters
 */
igraph_integer_t compute_communities_leiden(igraph_t* graph,
    igraph_vector_t* membership, igraph_real_t resolution, igraph_real_t beta);
