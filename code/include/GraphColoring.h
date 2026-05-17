#ifndef GRAPH_COLORING_H
#define GRAPH_COLORING_H

#include "Graph.h"
#include "DataStructures.h"
#include <stack>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

/**
 * @brief Builds and manages the interference graph from a set of webs.
 *
 * The interference graph is an undirected graph where each vertex represents
 * a web and edges connect webs whose live ranges overlap.
 */
class InterferenceGraph {
public:
    /**
     * @brief Constructs an interference graph with K available registers.
     * @param K Number of available CPU registers.
     */
    InterferenceGraph(int K);

    /**
     * @brief Builds the interference graph from a list of webs.
     * @details For each pair of webs, an edge is added if their live ranges overlap.
     * @par Time complexity
     * O(V^2) where V is the number of webs, since every pair is checked.
     * @param webs The vector of webs to build the graph from.
     */
    void build(const std::vector<Web>& webs);

    /** @brief Returns a const reference to the underlying graph. */
    const Graph<int>& getGraph() const { return graph; }
    /** @brief Returns a mutable reference to the underlying graph. */
    Graph<int>& getGraph() { return graph; }
    /** @brief Returns the number of available registers. */
    int getNumRegisters() const { return K; }

private:
    int K;              /**< Number of available registers. */
    Graph<int> graph;   /**< The interference graph (vertex IDs are web IDs). */
};

/**
 * @brief Implements graph coloring algorithms for register allocation.
 *
 * Provides static methods for Chaitin-style basic coloring and coloring
 * with spilling heuristics.
 */
class GraphColoring {
public:
    /**
     * @brief Performs basic Chaitin-style graph coloring.
     * @details
     * Phase 1 (Simplify): repeatedly pushes nodes with degree < K onto a stack and
     * removes them from the graph. If no such node exists, coloring fails.
     *
     * Phase 2 (Select): pops nodes from the stack and assigns the lowest
     * available color not used by any already-colored neighbor.
     *
     * @par Time complexity
     * O(V^2) where V is the number of vertices. The simplify phase may iterate
     * over all remaining vertices per round (O(V^2) worst-case), and the select
     * phase processes each vertex once, checking up to K colors (O(V*K)).
     *
     * @param ig The interference graph.
     * @param K  Number of available colors (registers).
     * @param colorAssignment Output map from vertex ID to assigned color.
     * @return True if the graph was successfully K-colored, false otherwise.
     */
    static bool basicColoring(Graph<int>& ig, int K,
                              std::unordered_map<int, int>& colorAssignment);

    /**
     * @brief Performs graph coloring with iterative spilling.
     * @details
     * Attempts basic coloring; if it fails, selects the vertex with the highest
     * effective degree (the "worst" node), marks it for spilling, and retries.
     * This process repeats until either a successful coloring is found or all
     * vertices are spilled.
     *
     * @par Time complexity
     * O(S * V^2) where V is the number of vertices and S is the number of
     * spilled vertices. In the worst case S = V, giving O(V^3).
     *
     * @param ig The interference graph.
     * @param K  Number of available colors (registers).
     * @param colorAssignment Output map from vertex ID to assigned color.
     * @param spilledWebs Output vector of vertex IDs that were spilled to memory.
     * @return True if coloring succeeded after spilling, false otherwise.
     */
    static bool coloringWithSpilling(Graph<int>& ig, int K,
                                     std::unordered_map<int, int>& colorAssignment,
                                     std::vector<int>& spilledWebs);

    /**
     * @brief Performs custom graph coloring using the Welsh-Powell algorithm (T2.4).
     * @details
     * Sorts vertices by degree in descending order and colors them greedily to minimize
     * register usage. Any vertices that cannot be colored with the available registers
     * are spilled to memory.
     * 
     * @par Time complexity
     * O(V * log V + V^2 * K) where V is the number of vertices and K is the number
     * of available colors (registers).
     *
     * @param graph Pointer to the interference graph.
     * @param maxRegisters Number of available colors (registers).
     * @return Map linking each web to its assigned register ("rX") or memory ("M").
     */
    template <class T>
    static std::map<T, std::string> allocateRegistersFree(Graph<T>* graph, int maxRegisters) {
        std::map<T, std::string> finalAllocation;
        std::vector<Vertex<T>*> vertices = graph->getVertexSet();

        if (vertices.empty()) return finalAllocation;

        std::sort(vertices.begin(), vertices.end(), [](Vertex<T>* a, Vertex<T>* b) {
            return a->getAdj().size() > b->getAdj().size();
        });

        std::map<T, int> colorMap;
        for (auto* v : vertices) {
            colorMap[v->getInfo()] = -1; 
        }

        int currentColor = 0;
        int uncoloredCount = vertices.size();

        while (uncoloredCount > 0 && currentColor < maxRegisters) {
            for (auto* v : vertices) {
                if (colorMap[v->getInfo()] == -1) {
                    bool safeToColor = true;

                    for (auto* edge : v->getAdj()) {
                        if (colorMap[edge->getDest()->getInfo()] == currentColor) {
                            safeToColor = false;
                            break;
                        }
                    }

                    if (safeToColor) {
                        colorMap[v->getInfo()] = currentColor;
                        uncoloredCount--;
                    }
                }
            }
            currentColor++;
        }

        for (auto* v : vertices) {
            T webInfo = v->getInfo();
            int assignedColor = colorMap[webInfo];

            if (assignedColor == -1) {
                finalAllocation[webInfo] = "M"; 
            } else {
                finalAllocation[webInfo] = "r" + std::to_string(assignedColor);
            }
        }

        return finalAllocation;
    }
};

#endif
