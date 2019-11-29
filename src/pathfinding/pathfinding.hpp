#ifndef CM_PATHFINDING_H
#define CM_PATHFINDING_H

#include <vector>

#include "graph.hpp"

/**
 * A step in the path returned from pathfinding
 */
struct path_step {
    // The next node in the path
    int node;

    // The direction in which the next node is
    direction dir;
};

/**
 * Finds the shortest path between two nodes in the given graph
 * 
 * @param g The graph to traverse
 * @param start The source node in the graph
 * @param end The end node in the graph
 * @return A path consisting of path steps  
 */
std::vector<path_step> find_shortest_path(graph& g, int start, int finish);

#endif