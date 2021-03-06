#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <igraph.h>

#include "display.h"
#include "quotient.h"
#include "sweep.h"
#include "vector.h"
#include "options.h"
#include "communities.h"

static void normal_double_sweep(igraph_t* graph)
{
    fprintf(stderr, "\n--------------------------------------------------\n");
    fprintf(stderr, "DOUBLE SWEEP ALGORITHM: \n");

    igraph_integer_t count;
    igraph_integer_t diameter_sweep;
    compute_statistics(graph, &count, &diameter_sweep);
    fprintf(stderr, "Diameter (double sweep): %d\n", diameter_sweep);
}

static void quotient_starting_double_sweep(igraph_t* graph, options_t* options)
{
    fprintf(stderr, "\n--------------------------------------------------\n");
    fprintf(stderr, "QUOTIENT STARTING DOUBLE SWEEP ALGORITHM: \n");

    igraph_vector_t membership;
    igraph_integer_t nb_clusters;

    if (options->use_louvain)
    {
        // Compute the communities using louvain
        fprintf(stderr, "Running Louvain\n");
        nb_clusters = compute_communities_louvain(graph, &membership);
    }
    else
    {
        // Compute the communities using leiden
        igraph_integer_t ecount = igraph_ecount(graph);
        igraph_real_t resolution = 1.0 / (2.0 * ecount);
        igraph_real_t beta = 0.01;

        fprintf(stderr, "Running Leiden (resolution: %f, beta: %f)\n",
                resolution, beta);

        nb_clusters = compute_communities_leiden(
                graph,
                &membership,
                resolution,
                beta);
    }

    // Print the number of clusters
    fprintf(stderr, "Clusters: %d\n", nb_clusters);

    // Print the modularity
    igraph_real_t leiden_modularity;
    igraph_modularity(graph, &membership, &leiden_modularity, NULL);
    fprintf(stderr, "Modularity: %f\n", leiden_modularity);

    if (options->print_membership)
    {
        // Print the communities
        fprintf(stderr, "Membership: ");
        vector_int_fprint(stderr, &membership);
        fprintf(stderr, "\n");
    }

    // Compute the cluster graph
    if (options->dot_colored)
    {
        // Write it as dot format on stdout
        write_graph_dot_clustered(graph, stdout, nb_clusters, &membership);
    }

    // Compute the quotient graph
    igraph_t quotient;
    quotient_graph(graph, nb_clusters, &membership, &quotient);

    // Compute the cluster statistics
    igraph_vector_t counts;
    igraph_vector_t diameters;
    compute_clusters_statistics(graph, nb_clusters, &membership, &counts,
                                &diameters);

    // Print the counts and diameters
    fprintf(stderr, "Counts: ");
    vector_int_fprint(stderr, &counts);
    fprintf(stderr, "\nDiameters: ");
    vector_int_fprint(stderr, &diameters);
    fprintf(stderr, "\n");

    // Destroy the statistics
    igraph_vector_destroy(&diameters);
    igraph_vector_destroy(&counts);

    // Display basic graph information
    graph_information("quotient", &quotient);

    if (options->dot_quotient)
    {
        // Write it as dot format on stdout
        igraph_write_graph_dot(&quotient, stdout);
    }

    // Get the diameter
    igraph_integer_t quotient_diameter;
    igraph_vector_t quotient_longest_path;
    igraph_vector_init(&quotient_longest_path, 0);
    igraph_diameter(&quotient, &quotient_diameter, NULL, NULL,
                    &quotient_longest_path, false, true);
    fprintf(stderr, "Quotient diameter: %d\n", quotient_diameter);
    fprintf(stderr, "Quotient longest path: ");
    vector_int_fprint(stderr, &quotient_longest_path);
    fprintf(stderr, "\n");

    // Destroy the quotient graph
    igraph_destroy(&quotient);

    // Take a starting community for double-sweep
    igraph_integer_t starting_community = VECTOR(quotient_longest_path)[0];

    // Destroy the longest path vector
    igraph_vector_destroy(&quotient_longest_path);

    if (options->quotient_try_all)
    {
        // Compute the double sweep starting from the vertices in a community
        igraph_integer_t diameter = double_sweep_from_community(graph,
            &membership, starting_community);
        fprintf(stderr, "Diameter (double sweep from starting community, "
                        "n: all): %d\n", diameter);
    }
    else
    {
        // Try the double sweep with different number of tries
        for (igraph_integer_t n = 1; n < 10; ++n)
        {
            // Compute the double sweep starting from the vertices in a community
            igraph_integer_t diameter = double_sweep_from_community_tries(graph,
                &membership, starting_community, n, false);
            fprintf(stderr, "Diameter (double sweep from starting community, "
                            "n: %d): %d\n", n, diameter);
        }

    }

    // Destroy the communities
    igraph_vector_destroy(&membership);
}

int main(int argc, char** argv)
{
    options_t options;
    if (!parse_options(argc, argv, &options))
        return 1;

    igraph_t graph;
    // Create a new graph
    igraph_read_graph_edgelist(&graph, options.input, 0, false);


    // Useful Print
    // ------------------------------
    if (options.dot_original)
    {
        // Write it as dot format on stdout
        igraph_write_graph_dot(&graph, stdout);
    }

    // General information
    // ------------------------------

    // Display basic graph information
    fprintf(stderr, "--------------------------------------------------\n");
    fprintf(stderr, "GENERAL INFORMATION: \n");
    graph_information(options.input_name, &graph);


    // Double Sweep Algorithm
    // ------------------------------
    normal_double_sweep(&graph);


    // Quotient Starting Double Sweep Algorithm
    // ------------------------------
    quotient_starting_double_sweep(&graph, &options);


    // Destroy the graph
    igraph_destroy(&graph);

    fclose(options.input);

    return 0;
}
