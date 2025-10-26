#ifndef LOCATIONGRAPH_H
#define LOCATIONGRAPH_H

#include "Location.h"
#include <unordered_map>
#include <vector>
#include <utility>
#include <limits>

class LocationGraph {
public:
    LocationGraph();

    // Add a location (node). If a node with same id exists, it will be overwritten.
    void addLocation(const Location& loc);

    // Add a directed edge from 'fromId' to 'toId' with given weight (distance/time)
    void addEdge(int fromId, int toId, double weight);

    // Get neighbors (id, weight)
    std::vector<std::pair<int,double>> neighbors(int id) const;

    // Get Location by id (returns nullptr if not found)
    const Location* getLocation(int id) const;

    // Shortest path using Dijkstra: returns vector of location ids in path order, empty if no path
    std::vector<int> shortestPath(int fromId, int toId) const;

private:
    std::unordered_map<int, Location> nodes_;
    std::unordered_map<int, std::vector<std::pair<int,double>>> adj_;
};


LocationGraph::LocationGraph() = default;

void LocationGraph::addLocation(const Location& loc) {
    nodes_[loc.getId()] = loc;
}

void LocationGraph::addEdge(int fromId, int toId, double weight) {
    // ensure nodes exist (no-op if absent; caller should add nodes first normally)
    adj_[fromId].push_back({toId, weight});
}

std::vector<std::pair<int,double>> LocationGraph::neighbors(int id) const {
    auto it = adj_.find(id);
    if (it == adj_.end()) return {};
    return it->second;
}

const Location* LocationGraph::getLocation(int id) const {
    auto it = nodes_.find(id);
    if (it == nodes_.end()) return nullptr;
    return &it->second;
}

std::vector<int> LocationGraph::shortestPath(int fromId, int toId) const {
    // Dijkstra over integer node ids
    const double INF = std::numeric_limits<double>::infinity();

    // distances and predecessor maps
    std::unordered_map<int,double> dist;
    std::unordered_map<int,int> prev;

    // min-heap (dist, node)
    using PD = std::pair<double,int>;
    std::priority_queue<PD, std::vector<PD>, std::greater<PD>> pq;

    // init
    for (const auto& kv : nodes_) {
        dist[kv.first] = INF;
    }
    if (dist.find(fromId) == dist.end() || dist.find(toId) == dist.end()) return {};

    dist[fromId] = 0.0;
    pq.push({0.0, fromId});

    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (u == toId) break;
        auto it = adj_.find(u);
        if (it == adj_.end()) continue;
        for (const auto& [v, w] : it->second) {
            double nd = d + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
                pq.push({nd, v});
            }
        }
    }

    if (dist[toId] == INF) return {};

    // reconstruct path
    std::vector<int> path;
    int cur = toId;
    while (true) {
        path.push_back(cur);
        if (cur == fromId) break;
        auto itp = prev.find(cur);
        if (itp == prev.end()) return {}; // no path
        cur = itp->second;
    }
    std::reverse(path.begin(), path.end());
    return path;
}


#include "../Headers/LocationGraph.h"
#include <queue>
#include <unordered_map>
#include <limits>

void LocationGraph::addLocation(const Location& loc) {
    nodes_[loc.getId()] = loc;
    // ensure adjacency entry exists
    if (adj_.find(loc.getId()) == adj_.end()) adj_[loc.getId()] = {};
}

void LocationGraph::addEdge(int fromId, int toId, double weight) {
    // ensure nodes exist (caller responsibility to add locations first is recommended)
    adj_[fromId].push_back({toId, weight});
    adj_[toId].push_back({fromId, weight}); // undirected
}

std::vector<std::pair<int,double>> LocationGraph::neighbors(int id) const {
    auto it = adj_.find(id);
    if (it == adj_.end()) return {};
    return it->second;
}

std::vector<int> LocationGraph::shortestPath(int startId, int endId) const {
    const double INF = std::numeric_limits<double>::infinity();
    std::unordered_map<int, double> dist;
    std::unordered_map<int, int> prev;

    // Min-heap (distance, node)
    using Pair = std::pair<double,int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;

    // initialize distances
    for (const auto& kv : nodes_) {
        dist[kv.first] = INF;
    }

    if (nodes_.find(startId) == nodes_.end() || nodes_.find(endId) == nodes_.end()) return {};

    dist[startId] = 0.0;
    pq.push({0.0, startId});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (u == endId) break;
        auto it = adj_.find(u);
        if (it == adj_.end()) continue;
        for (const auto& [v, w] : it->second) {
            double nd = d + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
                pq.push({nd, v});
            }
        }
    }

    if (dist[endId] == INF) return {};

    // reconstruct path
    std::vector<int> path;
    int cur = endId;
    while (cur != startId) {
        path.push_back(cur);
        auto itp = prev.find(cur);
        if (itp == prev.end()) return {}; // should not happen
        cur = itp->second;
    }
    path.push_back(startId);
    std::reverse(path.begin(), path.end());
    return path;
}


#endif // LOCATIONGRAPH_H
