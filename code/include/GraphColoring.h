#ifndef GRAPH_COLORING_H
#define GRAPH_COLORING_H

#include "Graph.h"
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include <algorithm>

struct Interval {
    int start;
    int end;

    bool overlaps(const Interval& other) const {
        return !(end < other.start || start > other.end);
    }
};

struct ProgramPoint {
    int line;
    char symbol;

    bool operator<(const ProgramPoint& other) const {
        return line < other.line;
    }
};

struct Web {
    int id;
    std::vector<Interval> liveRanges;
    std::string label;
    std::vector<ProgramPoint> points;
    std::string assignedResource;

    bool interferesWith(const Web& other) const {
        for (const auto& a : liveRanges) {
            for (const auto& b : other.liveRanges) {
                if (a.overlaps(b)) return true;
            }
        }
        return false;
    }
};

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
