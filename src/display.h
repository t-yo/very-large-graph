#pragma once

#include <igraph_datatype.h>

/**
 * @brief Display some basic information about a graph
 * @param name The name of the graph
 * @param graph The graph
 */
void graph_information(char* name, igraph_t* graph);

/**
 * @brief Write the graph as dot with clusters
 * @param graph The graph
 * @param output The output
 * @param nb_clusters The number of clusters
 * @param membership The membership of each vertex
 */
void write_graph_dot_clustered(igraph_t* graph, FILE* output,
    igraph_integer_t nb_clusters, igraph_vector_t* membership);

/**
 * @brief Write the graph as dot with a color associated to each node
 * @param graph The graph
 * @param weights The weights
 * @param output The output
 */
void write_graph_dot_node_colored(igraph_t* graph, igraph_vector_t* weights,
    FILE* output);
