
#include "pathfinding.hpp"

#include <utility>
#include <algorithm>

#include "graph.hpp"

std::vector<path_step> find_shortest_path(graph& g, int start, int end) {

    std::unordered_map<int, int> previous;
    std::unordered_map<int, int> dist;

    // Lambda function that compares costs of nodes by binding the dist map to it
    auto comparator = [&dist](int lhs, int rhs) { return dist[lhs] > dist[rhs]; };

    // Use comparator lambda above that uses dist to compare
    std::vector<int> queue;

    dist[start] = 0;
    queue.push_back(start);

    // Initialise all other nodes to infinite cost and add them to the priority queue
    for (int i = 0; i < g.get_nodes(); i++) {

        if (i == start) continue;

        dist[i] = std::numeric_limits<int>::max();
        queue.push_back(i);
    }

    std::make_heap(queue.begin(), queue.end(), comparator);

    while (!queue.empty()) {
        int current = queue.front();
        pop_heap(queue.begin(), queue.end(), comparator);
        queue.pop_back();

        for (const edge& e : g.get_edges(current)) {

            int new_dist = dist[current] + e.cost;

            if (new_dist < dist[e.end]) {
                dist[e.end] = new_dist;
                previous[e.end] = current;

                // Since costs have been updated, resort heap
                sort_heap(queue.begin(), queue.end(), comparator);
            }
        }
    }

    // All shortest paths have been found, unwind path
    int current = end;
    std::vector<path_step> path;

    while (current != start) {

        int prev = previous[current];

        // Add destination node (current) and the direction for each step
        edge e = g.get_edge(prev, current);
        path.push_back((path_step){current, e.dir});
        current = prev;
    }

    // Path is backwards so reverse it
    std::reverse(path.begin(), path.end());

    return path;
}
