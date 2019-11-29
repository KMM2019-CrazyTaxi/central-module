#ifndef CM_PATHFINDING_H
#define CM_PATHFINDING_H

#include <vector>

#include "graph.hpp"

struct path_step {
    int node;
    direction dir;
};

std::vector<path_step> find_shortest_path(graph& g, int start, int finish);

#endif