
#include "graph.hpp"

// Io thread has concat_bytes, should probably move it
#include "io_thread.hpp"

graph::graph(const uint8_t* buffer) {

    int index = 0;
    int nodes = (int) concat_bytes(buffer[index+1], buffer[index]);
    index += 2;

    for (int i = 0; i < nodes; i++) {
        int neighbor_count = (int) buffer[index++];
        
        std::vector<edge> neighbors(neighbor_count);
        
        for (int i = 0; i < neighbor_count; i++) {
            edge e;

            // Get the end of the edge
            e.end = (int) buffer[index++];
            
            // Get the cost of the edge
            e.cost = (int) concat_bytes(buffer[index+1], buffer[index]);
            index += 2;

            // Ignore stoppable
            index++;

            // Get the direction
            e.dir = (direction) buffer[index++];

            neighbors[i] = e;
        }

        g[i] = std::move(neighbors);
    }
}

std::vector<edge>& graph::get_edges(int node) {
    return g[node];
}

int graph::get_nodes() {
    return nodes;
}

edge graph::get_edge(int start, int end) {

    for (edge& e : get_edges(start)) {
        if (e.end == end) return e;
    }

    return (edge){0, 0, -1, (direction) 0};
}