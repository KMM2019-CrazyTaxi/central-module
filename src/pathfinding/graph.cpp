
#include "graph.hpp"

// Io thread has concat_bytes, should probably move it
#include "io_thread.hpp"

graph::graph() {

    nodes = 12;

    int DEFAULT_COST = 1;

    g[0].push_back((edge){0, 10, DEFAULT_COST, STRAIGHT});
    
    g[1].push_back((edge){1, 3, DEFAULT_COST, STRAIGHT});
    g[1].push_back((edge){1, 4, DEFAULT_COST, RIGHT});
    
    g[2].push_back((edge){2, 0, DEFAULT_COST, STRAIGHT});
    g[2].push_back((edge){2, 4, DEFAULT_COST, LEFT});

    g[3].push_back((edge){3, 9, DEFAULT_COST, STRAIGHT});

    g[4].push_back((edge){4, 6, DEFAULT_COST, STRAIGHT});

    g[5].push_back((edge){5, 0, DEFAULT_COST, LEFT});
    g[5].push_back((edge){5, 3, DEFAULT_COST, RIGHT});

    g[6].push_back((edge){6, 8, DEFAULT_COST, RIGHT});
    g[6].push_back((edge){6, 11, DEFAULT_COST, LEFT});

    g[7].push_back((edge){7, 5, DEFAULT_COST, STRAIGHT});

    g[8].push_back((edge){8, 1, DEFAULT_COST, STRAIGHT});

    g[9].push_back((edge){9, 8, DEFAULT_COST, STRAIGHT});
    g[9].push_back((edge){9, 7, DEFAULT_COST, RIGHT});

    g[10].push_back((edge){10, 7, DEFAULT_COST, LEFT});
    g[10].push_back((edge){10, 11, DEFAULT_COST, STRAIGHT});

    g[11].push_back((edge){11, 2, DEFAULT_COST, STRAIGHT});

}

graph::graph(const uint8_t* buffer) {

    int index = 0;
    nodes = (int) concat_bytes(buffer[index+1], buffer[index]);
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
