#include "communities.h"

#include <igraph.h>
#include <stdbool.h>

#include "vector.h"

igraph_integer_t compute_communities_louvain(igraph_t* graph,
    igraph_vector_t* membership)
{
    igraph_integer_t vcount = igraph_vcount(graph);

    // Initialize communities
    igraph_vector_init(membership, vcount);

    // Compute the communities
    igraph_community_multilevel(graph, NULL, membership,
        NULL, NULL);

    // Compute the number of communities
    igraph_integer_t nb_clusters = igraph_vector_max(membership) + 1;

    return nb_clusters;
}

igraph_integer_t compute_communities_leiden(igraph_t* graph,
    igraph_vector_t* membership, igraph_real_t resolution, igraph_real_t beta)
{
    igraph_integer_t vcount = igraph_vcount(graph);

    igraph_integer_t nb_clusters = vcount;
    igraph_real_t quality = 0;

    // Initialize communities
    igraph_vector_init(membership, vcount);

    // Initialize the degrees
    igraph_vector_t degrees;
    igraph_vector_init(&degrees, vcount);
    igraph_degree(graph, &degrees, igraph_vss_all(), IGRAPH_ALL, true);

    // Compute the communities
    igraph_community_leiden(graph, NULL, &degrees, resolution, beta,
        false, membership, &nb_clusters, &quality);

    // Destroy the degrees
    igraph_vector_destroy(&degrees);

    return nb_clusters;
}
