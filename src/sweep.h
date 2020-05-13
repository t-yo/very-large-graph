#pragma once

#include <igraph_datatype.h>

/**
 * @brief Compute statistics for each cluster
 * @param graph The graph
 * @param nb_clusters The number of clusters
 * @param membership The membership of each vertex
 * @param counts The number of vertices in each cluster (out)
 * @param diameters An approximation (double sweep) of the diameter of each cluster (out)
 */
void compute_clusters_statistics(igraph_t* graph, igraph_integer_t nb_clusters,
    igraph_vector_t* membership, igraph_vector_t* counts,
    igraph_vector_t* diameters);

/**
 * @brief Compute statistics for the graph
 * @param graph The graph
 * @param count The number of vertices in the graph (out)
 * @param diameter An approximation (double sweep) of the diameter of the graph
 */
void compute_statistics(igraph_t* graph, igraph_integer_t* count,
    igraph_integer_t* diameter);
