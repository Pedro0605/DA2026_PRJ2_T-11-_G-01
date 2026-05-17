#ifndef GRAPH_COLORING_H
#define GRAPH_COLORING_H

#include "Graph.h"
#include "DataStructures.h"
#include <stack>
#include <unordered_map>

class InterferenceGraph {
public:
    InterferenceGraph(int K);

    void build(const std::vector<Web>& webs);

    const Graph<int>& getGraph() const { return graph; }
    Graph<int>& getGraph() { return graph; }
    int getNumRegisters() const { return K; }

private:
    int K;
    Graph<int> graph;
};

class GraphColoring {
public:
    static bool basicColoring(Graph<int>& ig, int K,
                              std::unordered_map<int, int>& colorAssignment);

    static bool coloringWithSpilling(Graph<int>& ig, int K,
                                     std::unordered_map<int, int>& colorAssignment,
                                     std::vector<int>& spilledWebs);
};

#endif
