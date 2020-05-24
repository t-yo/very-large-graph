#include "sweep.h"

#include <stdbool.h>

#include <igraph.h>

#include "vector.h"

typedef struct sweep_result
{
    igraph_integer_t max_distance;
    igraph_integer_t last_vertex;
} sweep_result_t;

static igraph_bool_t sweep_callback(const igraph_t* graph,
    igraph_integer_t vid, igraph_integer_t pred, igraph_integer_t succ,
    igraph_integer_t rank, igraph_integer_t dist, void* extra)
{
    (void) graph;
    (void) pred;
    (void) succ;
    (void) rank;

    sweep_result_t* stats = extra;
    stats->max_distance = dist;
    stats->last_vertex = vid;

    return false;
}

static void compute_cluster_statistics(igraph_t* graph,
    igraph_vector_t* membership, igraph_integer_t cluster,
    igraph_integer_t* count, igraph_integer_t* diameter)
{
    igraph_integer_t vcount = igraph_vcount(graph);

    // Compute all the vertices in the cluster
    igraph_vector_t vertices;
    igraph_vector_init(&vertices, 0);

    for (igraph_integer_t i = 0; i < vcount; ++i)
    {
        igraph_integer_t current = VECTOR(*membership)[i];
        if (current == cluster)
        {
            igraph_vector_push_back(&vertices, i);
        }
    }

    *count = igraph_vector_size(&vertices);

    // First sweep
    sweep_result_t stats;
    igraph_bfs(graph, VECTOR(vertices)[0], NULL, IGRAPH_ALL, false,
        &vertices, NULL, NULL, NULL, NULL, NULL,
        NULL, sweep_callback, &stats);
    *diameter = stats.max_distance;

    // Double sweep
    igraph_bfs(graph, stats.last_vertex, NULL, IGRAPH_ALL, false,
        &vertices, NULL, NULL, NULL, NULL, NULL,
        NULL, sweep_callback, &stats);
    if (stats.max_distance > *diameter)
    {
        *diameter = stats.max_distance;
    }

    igraph_vector_destroy(&vertices);
}

void compute_clusters_statistics(igraph_t* graph, igraph_integer_t nb_clusters,
    igraph_vector_t* membership, igraph_vector_t* counts,
    igraph_vector_t* diameters)
{
    // Initialize the counts
    igraph_vector_init(counts, nb_clusters);

    // Initialize the diameters
    igraph_vector_init(diameters, nb_clusters);

    // Compute the statistics
    for (igraph_integer_t i = 0; i < nb_clusters; ++i)
    {
        igraph_integer_t count;
        igraph_integer_t diameter;
        compute_cluster_statistics(graph, membership, i, &count, &diameter);
        VECTOR(*counts)[i] = count;
        VECTOR(*diameters)[i] = diameter;
    }
}

void compute_statistics(igraph_t* graph, igraph_integer_t* count,
    igraph_integer_t* diameter)
{
    *count = igraph_vcount(graph);
    *diameter = double_sweep(graph);
}

igraph_integer_t double_sweep(igraph_t* graph)
{
    igraph_integer_t diameter;

    // First sweep
    sweep_result_t stats;
    igraph_bfs(graph, 0, NULL, IGRAPH_ALL, false,
        NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, sweep_callback, &stats);
    diameter = stats.max_distance;

    // Double sweep
    igraph_bfs(graph, stats.last_vertex, NULL, IGRAPH_ALL, false,
        NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, sweep_callback, &stats);
    if (stats.max_distance > diameter)
    {
        diameter = stats.max_distance;
    }

    return diameter;
}

igraph_integer_t double_sweep_from_community(igraph_t* graph,
    igraph_vector_t* membership, igraph_integer_t starting_community)
{
    igraph_integer_t vcount = igraph_vcount(graph);

    igraph_integer_t diameter = 0;

    for (igraph_integer_t i = 0; i < vcount; ++i)
    {
        igraph_integer_t community = VECTOR(*membership)[i];
        if (community == starting_community)
        {
            // First sweep
            sweep_result_t stats;
            igraph_bfs(graph, i, NULL, IGRAPH_ALL, false,
                NULL, NULL, NULL, NULL, NULL, NULL,
                NULL, sweep_callback, &stats);
            if (stats.max_distance > diameter)
            {
                diameter = stats.max_distance;
            }

            // Double sweep
            igraph_bfs(graph, stats.last_vertex, NULL, IGRAPH_ALL, false,
                NULL, NULL, NULL, NULL, NULL, NULL,
                NULL, sweep_callback, &stats);
            if (stats.max_distance > diameter)
            {
                diameter = stats.max_distance;
            }
        }
    }

    return diameter;
}
