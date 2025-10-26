#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <string>
#include <algorithm>

using namespace std;

const double INF = numeric_limits<double>::infinity();
using WeightVertexPair = pair<double, string>;
using Edge = pair<string, double>;

/**
 * @brief Represents the NUL map with specified buildings.
 */
class NULMapGraph {
private:
    map<string, vector<Edge>> adj_list;

public:
    // Adds a bidirectional edge (path) between two locations
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

    // Dijkstra's Shortest Path Algorithm
    map<string, string> dijkstra(const string& start_node, 
                                                map<string, double>& distances) const;
    
};

// ... Dijkstra's implementation (same as previous) ...
map<string, string> NULMapGraph::dijkstra(const string& start_node, 
                                                    map<string, double>& distances) const {
    
    priority_queue<WeightVertexPair, 
                        vector<WeightVertexPair>, 
                        greater<WeightVertexPair>> pq;
    map<string, string> previous_node;
    
    // Initialize distances
    for (const auto& pair : adj_list) {
        distances[pair.first] = INF;
    }
    
    if (adj_list.find(start_node) == adj_list.end()) {
        cerr << "Error: Start node '" << start_node << "' not found in the map." << endl;
        return previous_node;
    }

    distances[start_node] = 0.0;
    pq.push({0.0, start_node});

    while (!pq.empty()) {
        double current_dist = pq.top().first;
        string u = pq.top().second;
        pq.pop();

        if (current_dist > distances[u]) {
            continue;
        }

        auto it = adj_list.find(u);
        if (it != adj_list.end()) {
            for (const auto& edge : it->second) {
                string v = edge.first;
                double weight = edge.second;

                if (distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    previous_node[v] = u;
                    pq.push({distances[v], v});
                }
            }
        }
    }
    return previous_node;
}

/**
 * @brief Reconstructs and prints the path.
 */
void print_shortest_path(const string& start_node, 
                         const string& end_node, 
                         const map<string, string>& previous_node,
                         double final_distance) {
    
    if (previous_node.find(end_node) == previous_node.end() && start_node != end_node) {
        cout << "\n❌ Path not found.\n";
        return;
    }

    vector<string> path;
    string current = end_node;
    
    while (current != start_node) {
        path.push_back(current);
        current = previous_node.at(current);
    }
    path.push_back(start_node);
    reverse(path.begin(), path.end());

    cout << "\n🌟 **Shortest Path on NUL Campus**\n";
    cout << "------------------------------------------------------------------\n";
    cout << "Origin: **" << start_node << "**\n";
    cout << "Destination: **" << end_node << "**\n";
    cout << "Total Estimated Walk Time (Minutes): **" << final_distance << "**\n";
    cout << "Path: ";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i < path.size() - 1) {
            cout << " **->** ";
        }
    }
    cout << "\n------------------------------------------------------------------\n";
}

#endif // MAP_H