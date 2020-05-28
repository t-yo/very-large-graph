#pragma once

#include <stdbool.h>

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
 * @param diameter An approximation (double sweep) of the diameter of the graph (out)
 */
void compute_statistics(igraph_t* graph, igraph_integer_t* count,
    igraph_integer_t* diameter);

/**
 * @brief Compute the double sweep
 * @param graph The graph
 * @return An approximation of the diameter of the graph
 */
igraph_integer_t double_sweep(igraph_t* graph);

/**
 * @brief Compute the double sweep starting from a community
 * @param graph The graph
 * @param membership The membership of each vertex
 * @param starting_community The start of the double sweeps will be taken from this community
 * @return An approximation of the diameter of the graph
 */
igraph_integer_t double_sweep_from_community(igraph_t* graph,
    igraph_vector_t* membership, igraph_integer_t starting_community);

/**
 * @brief Compute the double sweep starting from a community with a certain number of tries
 * @param graph The graph
 * @param membership The membership of each vertex
 * @param starting_community The start of the double sweeps will be taken from this community
 * @param tries The number of different starts from the starting community
 *              (if the number of nodes in the community is lower, then tries=the number of nodes in the community)
 * @param verbose print the diameter for all the tries from 0 to 'tries'
 * @return An approximation of the diameter of the graph
 */
igraph_integer_t double_sweep_from_community_tries(igraph_t* graph,
    igraph_vector_t* membership, igraph_integer_t starting_community,
    igraph_integer_t tries, bool verbose);
