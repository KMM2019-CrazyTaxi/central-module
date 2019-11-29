
#include "graph.hpp"

graph::graph(uint8_t* buffer) {

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
    
}