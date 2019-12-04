#ifndef CM_GRAPH_H
#define CM_GRAPH_H

#include <unordered_map>
#include <vector>

/**
 * Directions used by the pathfinding and decision system
 */
enum direction {
    STRAIGHT = 0,
    LEFT = -1,
    RIGHT = 1
};

/**
 * An edge in a graph
 */
struct edge {

    // The start node of the edge
    int start;

    // The end node of the edge
    int end;

    // The cost of the edge
    int cost;

    // The direction of the edge
    direction dir;
};

/**
 * Class representing a weighted-directed graph
 */
class graph {

public:

    /**
     * Default constructor that creates the base graph with two junctions 
     */
    graph();

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
