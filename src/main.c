#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <igraph.h>

typedef struct options
{
    FILE* input;
    char* input_name;
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

    if (argc == 2)
    {
        options->input_name = argv[1];

        if (!(options->input = fopen(options->input_name, "r")))
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return false;
        }
    } else if (argc > 2)
    {
        fprintf(stderr, "Usage: %s [graph]\n", argv[0]);
        return false;
    }

    return true;
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

    printf("Membership: ");
    igraph_vector_print(&membership);
    printf("\n");

    // Destroy the communities
    igraph_vector_destroy(&membership);
}

void compute_communities_leiden(igraph_t* graph)
{
    fprintf(stderr, "Running Leiden\n");

    igraph_integer_t vcount = igraph_vcount(graph);
    igraph_integer_t ecount = igraph_ecount(graph);

    igraph_integer_t nb_clusters = vcount;
    igraph_real_t quality = 0;

    // Initialize communities
    igraph_vector_t membership;
    igraph_vector_init(&membership, vcount);

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
        false, &membership, &nb_clusters, &quality);

    fprintf(stderr, "Clusters: %d\n", nb_clusters);
    if (isnan(quality))
        fprintf(stderr, "Quality: nan\n");
    else
        fprintf(stderr, "Quality: %f\n", quality);

    igraph_real_t modularity;
    igraph_modularity(graph, &membership, &modularity, NULL);
    fprintf(stderr, "Modularity: %f\n", modularity);

    printf("Membership: ");
    igraph_vector_print(&membership);
    printf("\n");

    // Destroy the degrees
    igraph_vector_destroy(&degrees);

    // Destroy the communities
    igraph_vector_destroy(&membership);
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

    // Write it as dot format on stdout
    igraph_write_graph_dot(&graph, stdout);

    // Compute the communities using louvain
    compute_communities_louvain(&graph);

    // Compute the communities using leiden
    compute_communities_leiden(&graph);

    // Destroy the graph
    igraph_destroy(&graph);

    fclose(options.input);

    return 0;
}
