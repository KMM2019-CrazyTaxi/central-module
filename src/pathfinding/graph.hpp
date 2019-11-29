#ifndef CM_GRAPH_H
#define CM_GRAPH_H

#include <unordered_map>
#include <vector>

enum direction {
    STRAIGHT,
    LEFT,
    RIGHT
};

struct edge {
    int start;
    int end;
    int cost;
    direction dir;
};

class graph {

public:

    graph() = default;

    /**
     * Constructs a graph from the given byte buffer
     */
    graph(const uint8_t* buffer);

    /**
     * Returns the vector of edges 
     */
    std::vector<edge>& get_edges(int node);

    /**
     * Returns the number of nodes
     */
    int get_nodes();

    /**
     * Gets the directed edge that goes from start to end
     */
    edge get_edge(int start, int end);  

private:
    
    int nodes;
    std::unordered_map<int, std::vector<edge>> g;

};


#endif