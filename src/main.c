#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <igraph.h>

typedef struct options
{
    FILE* input;
    char* input_name;

    bool dot_original;
    bool dot_quotient;
} options_t;

void graph_information(char* name, igraph_t* graph)
{
    fprintf(stderr, "Name: %s\nVertices: %d\nEdges: %d\n", name,
        igraph_vcount(graph), igraph_ecount(graph));
}

bool parse_options(options_t* options, int argc, char** argv)
{
    options->input = stdin;
    options->input_name = "[stdin]";
    options->dot_original = false;
    options->dot_quotient = true;

    int current_arg = 1;
    while (current_arg < argc)
    {
        if (strcmp("--dot-original", argv[current_arg]) == 0)
        {
            options->dot_original = true;
            options->dot_quotient = false;
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

    // Compute the quotient graph
    igraph_t quotient;
    quotient_graph(&quotient, &graph, nb_clusters, &membership);

    // Display basic graph information
    graph_information("quotient", &quotient);

    if (options.dot_quotient)
    {
        // Write it as dot format on stdout
        igraph_write_graph_dot(&quotient, stdout);
    }

    // Destroy the quotient graph
    igraph_destroy(&quotient);

    // Destroy the communities
    igraph_vector_destroy(&membership);

    // Destroy the graph
    igraph_destroy(&graph);

    fclose(options.input);

    return 0;
}
