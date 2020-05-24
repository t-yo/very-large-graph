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

int main(int argc, char** argv)
{
    options_t options;
    if (!parse_options(argc, argv, &options))
        return 1;

    igraph_t graph;
    // Create a new graph
    igraph_read_graph_edgelist(&graph, options.input, 0, false);

    // General information
    // ------------------------------

    // Display basic graph information
    fprintf(stderr, "GENERAL INFORMATION: \n");
    graph_information(options.input_name, &graph);

    igraph_integer_t count;
    igraph_integer_t diameter_sweep;
    compute_statistics(&graph, &count, &diameter_sweep);
    fprintf(stderr, "Diameter (double sweep): %d\n", diameter_sweep);

    if (options.dot_original)
    {
        // Write it as dot format on stdout
        igraph_write_graph_dot(&graph, stdout);
    }

    // Communities computations
    // ------------------------------

    fprintf(stderr, "\nCOMMUNITIES: \n");

    igraph_vector_t membership;
    igraph_integer_t nb_clusters;

    if (options.use_leiden)
    {
        // Compute the communities using leiden
        igraph_integer_t ecount = igraph_ecount(&graph);
        igraph_real_t resolution = 1.0 / (2.0 * ecount);
        igraph_real_t beta = 0.01;

        fprintf(stderr, "Running Leiden (resolution: %f, beta: %f)\n",
                resolution, beta);

        nb_clusters = compute_communities_leiden(
                &graph,
                &membership,
                resolution,
                beta);
    }
    else
    {
        // Compute the communities using louvain
        fprintf(stderr, "Running Louvain\n");
        nb_clusters = compute_communities_louvain(&graph, &membership);
    }

    // Print the number of clusters
    fprintf(stderr, "Clusters: %d\n", nb_clusters);

    // Print the modularity
    igraph_real_t leiden_modularity;
    igraph_modularity(&graph, &membership, &leiden_modularity, NULL);
    fprintf(stderr, "Modularity: %f\n", leiden_modularity);

    if (options.print_membership)
    {
        // Print the communities
        fprintf(stderr, "Membership: ");
        vector_int_fprint(stderr, &membership);
        fprintf(stderr, "\n");
    }

    // Compute the cluster graph
    if (options.dot_colored)
    {
        // Write it as dot format on stdout
        write_graph_dot_clustered(&graph, stdout, nb_clusters, &membership);
    }

    // Compute the quotient graph
    igraph_t quotient;
    quotient_graph(&graph, nb_clusters, &membership, &quotient);

    // Compute the cluster statistics
    igraph_vector_t counts;
    igraph_vector_t diameters;
    compute_clusters_statistics(&graph, nb_clusters, &membership, &counts,
        &diameters);

    // Print the counts and diameters
    fprintf(stderr, "Counts: ");
    vector_int_fprint(stderr, &counts);
    fprintf(stderr, "\nDiameters: ");
    vector_int_fprint(stderr, &diameters);
    fprintf(stderr, "\n");

    // Display basic graph information
    graph_information("quotient", &quotient);

    if (options.dot_quotient)
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

    // Take a starting community for double-sweep
    igraph_integer_t starting_community = VECTOR(quotient_longest_path)[0];

    // Compute the double sweep starting from the vertices in a community
    igraph_integer_t diameter = double_sweep_from_community(&graph, &membership,
        starting_community);
    fprintf(stderr, "Diameter (double sweep from starting community): "
                    "%d\n", diameter);

    // Destroy the longest path vector
    igraph_vector_destroy(&quotient_longest_path);

    // Destroy the statistics
    igraph_vector_destroy(&diameters);
    igraph_vector_destroy(&counts);

    // Destroy the quotient graph
    igraph_destroy(&quotient);

    // Destroy the communities
    igraph_vector_destroy(&membership);

    // Destroy the graph
    igraph_destroy(&graph);

    fclose(options.input);

    return 0;
}
