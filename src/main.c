#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <igraph.h>

#include "display.h"

typedef struct options
{
    FILE* input;
    char* input_name;

    bool dot_original;
    bool dot_quotient;
    bool dot_colored;
} options_t;


bool parse_options(options_t* options, int argc, char** argv)
{
    options->input = stdin;
    options->input_name = "[stdin]";
    options->dot_original = false;
    options->dot_quotient = true;
    options->dot_colored = false;

    int current_arg = 1;
    while (current_arg < argc)
    {
        if (strcmp("--dot-original", argv[current_arg]) == 0)
        {
            options->dot_original = true;
            options->dot_quotient = false;
            options->dot_colored = false;
        } else if (strcmp("--dot-colored", argv[current_arg]) == 0)
        {
            options->dot_original = false;
            options->dot_quotient = false;
            options->dot_colored = true;
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
    fprintf(stderr, "Membership:");
    for (igraph_integer_t i = 0; i < vcount; ++i)
    {
        fprintf(stderr, " %d", (igraph_integer_t)VECTOR(membership)[i]);
    }
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
    fprintf(stderr, "Membership:");
    for (igraph_integer_t i = 0; i < vcount; ++i)
    {
        fprintf(stderr, " %d", (igraph_integer_t)VECTOR(*membership)[i]);
    }
    fprintf(stderr, "\n");

    // Destroy the degrees
    igraph_vector_destroy(&degrees);

    return nb_clusters;
}

void quotient_graph(igraph_t* result, igraph_t* graph,
    igraph_integer_t nb_clusters, igraph_vector_t* membership)
{
    // Initialize the graph
    igraph_small(result, nb_clusters, IGRAPH_UNDIRECTED, -1);

    // Initialize the selector
    igraph_es_t selector;
    igraph_es_all(&selector, IGRAPH_EDGEORDER_ID);

    // Initialize the iterator
    igraph_eit_t iterator;
    igraph_eit_create(graph, selector, &iterator);

    while (!IGRAPH_EIT_END(iterator))
    {
        igraph_integer_t from, to;
        igraph_edge(graph, IGRAPH_EIT_GET(iterator), &from, &to);

        igraph_integer_t quotient_from = VECTOR(*membership)[from];
        igraph_integer_t quotient_to = VECTOR(*membership)[to];

        if (quotient_from != quotient_to)
        {
            igraph_bool_t connected;
            igraph_are_connected(result, quotient_from, quotient_to, &connected);
            if (!connected)
            {
                igraph_add_edge(result, quotient_from, quotient_to);
            }
        }

        IGRAPH_EIT_NEXT(iterator);
    }

    // Destroy the iterator
    igraph_eit_destroy(&iterator);

    // Destroy the selector
    igraph_es_destroy(&selector);
}

typedef struct cluster_statistics_bfs_result_t
{
    igraph_integer_t max_distance;
    igraph_integer_t last_vertex;
} cluster_statistics_bfs_result_t;

igraph_bool_t compute_cluster_statistics_callback(const igraph_t* graph,
    igraph_integer_t vid, igraph_integer_t pred, igraph_integer_t succ,
    igraph_integer_t rank, igraph_integer_t dist, void* extra)
{
    (void) graph;
    (void) pred;
    (void) succ;
    (void) rank;

    cluster_statistics_bfs_result_t* stats = extra;
    stats->max_distance = dist;
    stats->last_vertex = vid;

    return false;
}

void compute_cluster_statistics(igraph_t* graph,
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
    cluster_statistics_bfs_result_t  stats;
    igraph_bfs(graph, VECTOR(vertices)[0], NULL, IGRAPH_ALL, false,
        &vertices, NULL, NULL, NULL, NULL, NULL,
        NULL, compute_cluster_statistics_callback, &stats);
    *diameter = stats.max_distance;

    // Double sweep
    igraph_bfs(graph, stats.last_vertex, NULL, IGRAPH_ALL, false,
        &vertices, NULL, NULL, NULL, NULL, NULL,
        NULL, compute_cluster_statistics_callback, &stats);
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

    // Print the diameters
    fprintf(stderr, "Counts:");
    for (igraph_integer_t i = 0; i < nb_clusters; ++i)
    {
        fprintf(stderr, " %d", (igraph_integer_t)VECTOR(*counts)[i]);
    }
    fprintf(stderr, "\nDiameters:");
    for (igraph_integer_t i = 0; i < nb_clusters; ++i)
    {
        fprintf(stderr, " %d", (igraph_integer_t)VECTOR(*diameters)[i]);
    }
    fprintf(stderr, "\n");
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
    quotient_graph(&quotient, &graph, nb_clusters, &membership);

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
    igraph_integer_t diameter;
    igraph_vector_t longest_path;
    igraph_vector_init(&longest_path, 0);
    igraph_diameter(&quotient, &diameter, NULL, NULL, &longest_path,
        false, true);
    fprintf(stderr, "Quotient diameter: %d\n", diameter);
    fprintf(stderr, "Quotient longest path:");
    for (igraph_integer_t i = 0; i < igraph_vector_size(&longest_path); ++i)
    {
        fprintf(stderr, " %d", (igraph_integer_t)VECTOR(longest_path)[i]);
    }
    fprintf(stderr, "\n");

    // Destroy the longest path vector
    igraph_vector_destroy(&longest_path);

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
