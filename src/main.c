#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <igraph.h>

#include "display.h"
#include "quotient.h"
#include "sweep.h"
#include "vector.h"

typedef struct options
{
    FILE* input;
    char* input_name;

    bool dot_original;
    bool dot_quotient;
    bool dot_colored;
    bool dot_weighted_quotient;
} options_t;


bool parse_options(options_t* options, int argc, char** argv)
{
    options->input = stdin;
    options->input_name = "[stdin]";
    options->dot_original = false;
    options->dot_quotient = false;
    options->dot_colored = false;
    options->dot_weighted_quotient = false;

    int current_arg = 1;
    while (current_arg < argc)
    {
        if (strcmp("--dot-original", argv[current_arg]) == 0)
        {
            options->dot_original = true;
            options->dot_quotient = false;
            options->dot_colored = false;
            options->dot_weighted_quotient = false;
        } else if (strcmp("--dot-colored", argv[current_arg]) == 0)
        {
            options->dot_original = false;
            options->dot_quotient = false;
            options->dot_colored = true;
            options->dot_weighted_quotient = false;
        } else if (strcmp("--dot-quotient", argv[current_arg]) == 0)
        {
            options->dot_original = false;
            options->dot_quotient = true;
            options->dot_colored = false;
            options->dot_weighted_quotient = false;
        } else if (strcmp("--dot-weighted-quotient", argv[current_arg]) == 0)
        {
            options->dot_original = false;
            options->dot_quotient = false;
            options->dot_colored = false;
            options->dot_weighted_quotient = true;
        } else
        {
            break;
        }
        current_arg += 1;
    }

    // If we don't have the filename at the end
    if (current_arg == argc)
    {
        return true;
    }

    // If we have the filename at the end
    if (current_arg + 1 == argc)
    {
        options->input_name = argv[current_arg];

        if (!(options->input = fopen(options->input_name, "r")))
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return false;
        }

        return true;
    }

    // If we have more arguments
    fprintf(stderr, "Usage: %s [options] [graph]\n", argv[0]);
    return false;
}

void compute_communities_louvain(igraph_t* graph)
{
    fprintf(stderr, "Running Louvain\n");

    igraph_integer_t vcount = igraph_vcount(graph);

    // Initialize communities
    igraph_vector_t membership;
    igraph_vector_init(&membership, vcount);

    // Compute the communities
    igraph_community_multilevel(graph, NULL, &membership,
        NULL, NULL);

    fprintf(stderr, "Clusters: %d\n",
        (int) igraph_vector_max(&membership) + 1);

    igraph_real_t modularity;
    igraph_modularity(graph, &membership, &modularity, NULL);
    fprintf(stderr, "Modularity: %f\n", modularity);

    // Print the communities
    fprintf(stderr, "Membership: ");
    vector_int_fprint(stderr, &membership);
    fprintf(stderr, "\n");

    // Destroy the communities
    igraph_vector_destroy(&membership);
}

igraph_integer_t compute_communities_leiden(igraph_t* graph,
    igraph_vector_t* membership)
{
    fprintf(stderr, "Running Leiden\n");

    igraph_integer_t vcount = igraph_vcount(graph);
    igraph_integer_t ecount = igraph_ecount(graph);

    igraph_integer_t nb_clusters = vcount;
    igraph_real_t quality = 0;

    // Initialize communities
    igraph_vector_init(membership, vcount);

    // Initialize the degrees
    igraph_vector_t degrees;
    igraph_vector_init(&degrees, vcount);
    igraph_degree(graph, &degrees, igraph_vss_all(), IGRAPH_ALL, true);

    // Compute the resolution
    igraph_real_t resolution = 1.0 / (2.0 * ecount);
    igraph_real_t beta = 0.01;
    fprintf(stderr, "Resolution: %f\nBeta: %f\n", resolution, beta);

    // Compute the communities
    igraph_community_leiden(graph, NULL, &degrees, resolution, beta,
        false, membership, &nb_clusters, &quality);

    fprintf(stderr, "Clusters: %d\n", nb_clusters);
    if (isnan(quality))
        fprintf(stderr, "Quality: nan\n");
    else
        fprintf(stderr, "Quality: %f\n", quality);

    igraph_real_t modularity;
    igraph_modularity(graph, membership, &modularity, NULL);
    fprintf(stderr, "Modularity: %f\n", modularity);

    // Print the communities
    fprintf(stderr, "Membership: ");
    vector_int_fprint(stderr, membership);
    fprintf(stderr, "\n");

    // Destroy the degrees
    igraph_vector_destroy(&degrees);

    return nb_clusters;
}

void weight_quotient_graph(igraph_t* quotient, igraph_vector_t* diameters,
    igraph_t* result_graph, igraph_vector_t* result_weights)
{
    igraph_integer_t vcount = igraph_vcount(quotient);

    // Initialize the weights
    igraph_vector_init(result_weights, 0);

    // Initialize the output graph
    igraph_empty(result_graph, vcount, true);

    // Initialize the selector
    igraph_es_t selector;
    igraph_es_all(&selector, IGRAPH_EDGEORDER_ID);

    // Initialize the iterator
    igraph_eit_t iterator;
    igraph_eit_create(quotient, selector, &iterator);

    while (!IGRAPH_EIT_END(iterator))
    {
        igraph_integer_t from, to;
        igraph_edge(quotient, IGRAPH_EIT_GET(iterator), &from, &to);

        igraph_add_edge(result_graph, from, to);
        igraph_vector_push_back(result_weights, VECTOR(*diameters)[from]);

        igraph_add_edge(result_graph, to, from);
        igraph_vector_push_back(result_weights, VECTOR(*diameters)[to]);

        IGRAPH_EIT_NEXT(iterator);
    }
}


int main(int argc, char** argv)
{
    options_t options;
    if (!parse_options(&options, argc, argv))
        return 1;

    igraph_t graph;
    // Create a new graph
    igraph_read_graph_edgelist(&graph, options.input, 0, false);

    // Display basic graph information
    graph_information(options.input_name, &graph);

    igraph_integer_t count;
    igraph_integer_t diameter;
    compute_statistics(&graph, &count, &diameter);
    fprintf(stderr, "Diameter (double sweep): %d\n", diameter);


    if (options.dot_original)
    {
        // Write it as dot format on stdout
        igraph_write_graph_dot(&graph, stdout);
    }

    // Compute the communities using louvain
    // compute_communities_louvain(&graph);

    // Compute the communities using leiden
    igraph_vector_t membership;
    igraph_integer_t nb_clusters = compute_communities_leiden(&graph, &membership);

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

    igraph_t weighted_quotient;
    igraph_vector_t weighted_quotient_weights;
    weight_quotient_graph(&quotient, &diameters,
        &weighted_quotient, &weighted_quotient_weights);

    if (options.dot_weighted_quotient)
    {
        // Write it as dot format on stdout
        write_graph_dot_node_colored(&weighted_quotient,
            &weighted_quotient_weights, stdout);
    }

    // Destroy the weighted quotient graph
    igraph_vector_destroy(&weighted_quotient_weights);
    igraph_destroy(&weighted_quotient);

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
