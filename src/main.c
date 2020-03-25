#include <stdio.h>
#include <igraph.h>

int main()
{
    igraph_t graph;
    // Create a new graph
    igraph_star(&graph, 10, IGRAPH_STAR_UNDIRECTED, 5);

    // Write it as dot format on stdout
    igraph_write_graph_dot(&graph, stdout);;

    // Destroy the graph
    igraph_destroy(&graph);
    return 0;
}
