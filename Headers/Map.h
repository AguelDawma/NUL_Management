#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

// Infinity value for distances
const double INF = numeric_limits<double>::infinity();
using WeightVertexPair = pair<double, string>;
using Edge = pair<string, double>;

/**
 * @brief Represents the NUL map with specified buildings.
 */
class NULMapGraph {
private:
    map<string, vector<Edge>> adj_list;

    vector<string> reconstruct_path(const string& start_node, const string& end_node,
                                    const map<string, string>& previous_node) const {
        vector<string> path;
        string current = end_node;

        // Traverse backwards from end_node to start_node
        while (current != start_node) {
            path.push_back(current);
            if (previous_node.find(current) == previous_node.end()) {
                return {};
            }
            current = previous_node.at(current);
        }
        path.push_back(start_node);
        reverse(path.begin(), path.end());
        return path;
    }

public:
    void add_path(const string& u, const string& v, double weight) {
        adj_list[u].push_back({v, weight});
        adj_list[v].push_back({u, weight}); // Undirected graph
    }

    // Retrieves all nodes (buildings) in the map
    vector<string> get_nodes() const {
        vector<string> nodes;
        for (const auto& pair : adj_list) {
            nodes.push_back(pair.first);
        }
        return nodes;
    }

    double get_edge_weight(const string& u, const string& v) const {
        if (adj_list.find(u) != adj_list.end()) {
            for (const auto& edge : adj_list.at(u)) {
                if (edge.first == v) {
                    return edge.second;
                }
            }
        }
        return INF; // No direct edge
    }

    pair<vector<string>, double> dijkstra_shortest_path(const string& start_node, const string& end_node) const {
        // Priority Queue: Min-heap to store {distance, vertex}
        // Use greater<> for min-heap on the first element (distance)
        priority_queue<WeightVertexPair, vector<WeightVertexPair>, greater<WeightVertexPair>> pq;
        map<string, double> distance;
        map<string, string> previous_node; // To reconstruct the path

        // Initialize distances
        for (const auto& pair : adj_list) {
            distance[pair.first] = INF;
        }

        distance[start_node] = 0.0;
        pq.push({0.0, start_node});

        while (!pq.empty()) {
            double d_u = pq.top().first;
            string u = pq.top().second;
            pq.pop();

            // Optimization: If the extracted distance is greater than the known shortest distance, skip
            if (d_u > distance[u]) {
                continue;
            }

            // Early exit if destination is reached
            if (u == end_node) {
                break;
            }

            // Iterate over neighbors of u
            if (adj_list.find(u) != adj_list.end()) {
                for (const auto& edge : adj_list.at(u)) {
                    string v = edge.first;
                    double weight_uv = edge.second;

                    // Relax the edge
                    if (distance[u] + weight_uv < distance[v]) {
                        distance[v] = distance[u] + weight_uv;
                        previous_node[v] = u;
                        pq.push({distance[v], v});
                    }
                }
            }
        }

        double final_distance = distance.count(end_node) ? distance.at(end_node) : INF;
        vector<string> path = reconstruct_path(start_node, end_node, previous_node);

        return {path, final_distance};
    }

    /**
     * @brief Prints the shortest path details clearly.
     * @param start_node The name of the starting building.
     * @param end_node The name of the destination building.
     * @param path The vector of nodes forming the shortest path.
     * @param final_distance The total walk time/distance.
     */
    void print_shortest_path(const string& start_node, const string& end_node,
                             const vector<string>& path, double final_distance) const {

        if (final_distance == INF) {
            cout << "\n------------------------------------------------------------------\n";
            cout << "No path found between " << start_node << " and " << end_node << ".\n";
            cout << "------------------------------------------------------------------\n";
            return;
        }

        cout << "\n======================== SHORTEST PATH FOUND =======================\n";
        cout << "Origin: " << start_node << "\n";
        cout << "Destination: " << end_node << "\n";
        cout << "Total Estimated Walk Time: " << fixed << setprecision(1) << final_distance << " minutes\n";
        cout << "==================================================================\n";

        cout << "Route Details:\n";
        double cumulative_distance = 0.0;

        cout << "Start: " << path[0] << "\n";

        // Iterate through the path, showing edges and intermediate nodes
        for (size_t i = 0; i < path.size() - 1; ++i) {
            const string& u = path[i];
            const string& v = path[i + 1];

            // Get the distance for this segment using the graph object
            double segment_weight = get_edge_weight(u, v);
            cumulative_distance += segment_weight;

            // Print the Edge (Walk)
            cout << "   Walk from " << u << " to " << v
                 << " (Time: " << fixed << setprecision(1) << segment_weight << " min)";

            // Show the cumulative total
            cout << " [Cumulative: " << fixed << setprecision(1) << cumulative_distance << " min]\n";
        }

        cout << "End: " << path.back() << "\n";
        cout << "------------------------------------------------------------------\n";
    }
};

#endif // MAP_H