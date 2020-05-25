#include "display.h"

#include <igraph.h>

#include "color.h"

void graph_information(char* name, igraph_t* graph)
{
    fprintf(stderr, "Name: %s\n", name);
    fprintf(stderr, "Vertices: %d\n", igraph_vcount(graph));
    fprintf(stderr, "Edges: %d\n", igraph_ecount(graph));
    fprintf(stderr, "Directed: %d\n", igraph_is_directed(graph));
}

static void write_graph_clustered_vertices(igraph_t* graph, FILE* output,
    igraph_integer_t nb_clusters, igraph_vector_t* membership,
    igraph_integer_t cluster)
{
    // Initialize the selector
    igraph_vs_t selector;
    igraph_vs_all(&selector);

    // initialize the iterator
    igraph_vit_t iterator;
    igraph_vit_create(graph, selector, &iterator);

    if (cluster == nb_clusters) {
        // Add all vertices
        while (!IGRAPH_VIT_END(iterator))
        {
            igraph_integer_t vertex = IGRAPH_VIT_GET(iterator);
            fprintf(output, "    %d;\n", vertex);
            IGRAPH_VIT_NEXT(iterator);
        }
    } else {
        // Add all vertices
        while (!IGRAPH_VIT_END(iterator))
        {
            igraph_integer_t vertex = IGRAPH_VIT_GET(iterator);
            igraph_integer_t quotient = VECTOR(*membership)[vertex];
            if (quotient == cluster) {
                fprintf(output, "        %d;\n", vertex);
            }
            IGRAPH_VIT_NEXT(iterator);
        }
    }


    // Destroy the iterator
    igraph_vit_destroy(&iterator);

    // Destroy the selector
    igraph_vs_destroy(&selector);
}

static void write_graph_clustered_edges(igraph_t* graph, FILE* output,
    igraph_integer_t nb_clusters, igraph_vector_t* membership,
    igraph_integer_t cluster)
{
    // Initialize the selector
    igraph_es_t selector;
    igraph_es_all(&selector, IGRAPH_EDGEORDER_ID);

    // Initialize the iterator
    igraph_eit_t iterator;
    igraph_eit_create(graph, selector, &iterator);

    if (cluster == nb_clusters) {
        // Add all edges
        while (!IGRAPH_EIT_END(iterator))
        {
            igraph_integer_t from, to;
            igraph_edge(graph, IGRAPH_EIT_GET(iterator), &from, &to);

            igraph_integer_t quotient_from = VECTOR(*membership)[from];
            igraph_integer_t quotient_to = VECTOR(*membership)[to];

            if (quotient_from != quotient_to)
            {
                fprintf(output, "    %d -- %d;\n", from, to);
            }

            IGRAPH_EIT_NEXT(iterator);
        }
    } else {
        // Add all edges
        while (!IGRAPH_EIT_END(iterator))
        {
            igraph_integer_t from, to;
            igraph_edge(graph, IGRAPH_EIT_GET(iterator), &from, &to);

            igraph_integer_t quotient_from = VECTOR(*membership)[from];
            igraph_integer_t quotient_to = VECTOR(*membership)[to];

            if (quotient_from == cluster && quotient_to == cluster)
            {
                fprintf(output, "        %d -- %d;\n", from, to);
            }

            IGRAPH_EIT_NEXT(iterator);
        }
    }

    // Destroy the iterator
    igraph_eit_destroy(&iterator);

    // Destroy the selector
    igraph_es_destroy(&selector);
}

void write_graph_dot_clustered(igraph_t* graph, FILE* output,
    igraph_integer_t nb_clusters, igraph_vector_t* membership)
{
    fprintf(output, "/* Created manually */\n");
    fprintf(output, "graph {\n");

    for (igraph_integer_t cluster = 0; cluster < nb_clusters; ++cluster) {
        fprintf(output, "    subgraph cluster_%d {\n", cluster);
        u_int32_t color = generate_color(cluster, nb_clusters);
        fprintf(output, "        color=\"#%06x\";\n", color);
        fprintf(output, "        edge [color=\"#%06x\"];\n", color);
        fprintf(output, "\n");
        write_graph_clustered_vertices(graph, output, nb_clusters, membership,
            cluster);
        fprintf(output, "\n");
        write_graph_clustered_edges(graph, output, nb_clusters, membership,
            cluster);
        fprintf(output, "    }\n");
        fprintf(output, "\n");
    }

    write_graph_clustered_edges(graph, output, nb_clusters, membership,
        nb_clusters);

    fprintf(output, "}\n");
}

static void write_graph_dot_node_colored_vertices(igraph_t* graph, FILE* output)
{
    igraph_integer_t vcount = igraph_vcount(graph);

    // Initialize the selector
    igraph_vs_t selector;
    igraph_vs_all(&selector);

    // Initialize the iterator
    igraph_vit_t iterator;
    igraph_vit_create(graph, selector, &iterator);

    while (!IGRAPH_VIT_END(iterator))
    {
        igraph_integer_t vertex = IGRAPH_VIT_GET(iterator);
        u_int32_t color = generate_color(vertex, vcount);

        fprintf(output, "    %d [color=\"#%06x\"];\n", vertex, color);

        IGRAPH_VIT_NEXT(iterator);
    }
}

static void write_graph_dot_node_colored_edges(igraph_t* graph, igraph_vector_t* weights,
    FILE* output)
{
    igraph_integer_t vcount = igraph_vcount(graph);

    // Initialize the selector
    igraph_es_t selector;
    igraph_es_all(&selector, IGRAPH_EDGEORDER_ID);

    // Initialize the iterator
    igraph_eit_t iterator;
    igraph_eit_create(graph, selector, &iterator);

    while (!IGRAPH_EIT_END(iterator))
    {
        igraph_integer_t edge = IGRAPH_EIT_GET(iterator);
        igraph_integer_t from, to;
        igraph_edge(graph, edge, &from, &to);
        u_int32_t color = generate_color(from, vcount);

        fprintf(output, "    %d -> %d [color=\"#%06x\", fontcolor=\"#%06x\", label=\"%d\"];\n",
            from, to, color,color, (igraph_integer_t) VECTOR(*weights)[edge]);

        IGRAPH_EIT_NEXT(iterator);
    }
}

void write_graph_dot_node_colored(igraph_t* graph, igraph_vector_t* weights,
    FILE* output)
{
    fprintf(output, "/* Created manually */\n");
    fprintf(output, "digraph {\n");
    write_graph_dot_node_colored_vertices(graph, output);
    write_graph_dot_node_colored_edges(graph, weights, output);
    fprintf(output, "}\n");
}
