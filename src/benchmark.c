#include <stdio.h>
#include <string.h>

#include <igraph.h>

#include "display.h"
#include "quotient.h"
#include "sweep.h"
#include "communities.h"
#include "benchmark.h"

static igraph_integer_t normal_double_sweep(igraph_t* graph, bool verbose)
{
    (void) verbose;

    return double_sweep(graph);
}

static igraph_integer_t quotient_starting_double_sweep_louvain(igraph_t* graph,
        bool verbose)
{
    // Compute the communities using louvain
    igraph_vector_t membership;
    igraph_integer_t nb_clusters =
            compute_communities_louvain(graph, &membership);

    // Compute the quotient graph
    igraph_t quotient;
    quotient_graph(graph, nb_clusters, &membership, &quotient);

    // Get the exact diameter
    igraph_integer_t quotient_diameter;
    igraph_vector_t quotient_longest_path;
    igraph_vector_init(&quotient_longest_path, 0);
    igraph_diameter(&quotient, &quotient_diameter, NULL, NULL,
                    &quotient_longest_path, false, true);

    // Destroy the quotient graph
    igraph_destroy(&quotient);

    // Take a starting community for double-sweep
    igraph_integer_t starting_community = VECTOR(quotient_longest_path)[0];

    // Destroy the longest path vector
    igraph_vector_destroy(&quotient_longest_path);

    // Compute the double sweep starting from the vertices in a community
    igraph_integer_t diameter = double_sweep_from_community_tries(graph,
        &membership, starting_community, 3, verbose);

    // Destroy the communities
    igraph_vector_destroy(&membership);

    return diameter;
}

static igraph_integer_t quotient_starting_double_sweep_leiden(igraph_t* graph,
        bool verbose)
{
    // Compute the communities using leiden
    igraph_integer_t ecount = igraph_ecount(graph);
    igraph_real_t resolution = 1.0 / (2.0 * ecount);
    igraph_real_t beta = 0.01;
    igraph_vector_t membership;
    igraph_integer_t nb_clusters = compute_communities_leiden(graph,
        &membership, resolution, beta);

    // Compute the quotient graph
    igraph_t quotient;
    quotient_graph(graph, nb_clusters, &membership, &quotient);

    // Get the exact diameter
    igraph_integer_t quotient_diameter;
    igraph_vector_t quotient_longest_path;
    igraph_vector_init(&quotient_longest_path, 0);
    igraph_diameter(&quotient, &quotient_diameter, NULL, NULL,
        &quotient_longest_path, false, true);

    // Destroy the quotient graph
    igraph_destroy(&quotient);

    // Take a starting community for double-sweep
    igraph_integer_t starting_community = VECTOR(quotient_longest_path)[0];

    // Destroy the longest path vector
    igraph_vector_destroy(&quotient_longest_path);

    // Compute the double sweep starting from the vertices in a community
    igraph_integer_t diameter = double_sweep_from_community_tries(graph,
        &membership, starting_community, 3, verbose);

    // Destroy the communities
    igraph_vector_destroy(&membership);

    return diameter;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [graph]\n", argv[0]);
        return 1;
    }

    int min_tries = 3;
    int min_time = 60;

    BENCHMARK(argv[1],
            normal_double_sweep,
            min_tries,
            min_time);

    BENCHMARK(argv[1],
              quotient_starting_double_sweep_leiden,
              min_tries,
              min_time);

    BENCHMARK(argv[1],
            quotient_starting_double_sweep_louvain,
            min_tries,
            min_time);

    return 0;
}
