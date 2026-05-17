#include "GraphColoring.h"
#include <unordered_set>
#include <algorithm>

/**
 * @brief Implementation of InterferenceGraph::InterferenceGraph.
 * @see GraphColoring.h for full documentation.
 */
InterferenceGraph::InterferenceGraph(int K) : K(K) {}

/**
 * @brief Implementation of InterferenceGraph::build.
 * @see GraphColoring.h for full documentation.
 */
void InterferenceGraph::build(const std::vector<Web>& webs) {
    for (const auto& w : webs) {
        graph.addVertex(w.id);
    }

    for (size_t i = 0; i < webs.size(); i++) {
        for (size_t j = i + 1; j < webs.size(); j++) {
            if (webs[i].interferesWith(webs[j])) {
                graph.addFlowEdge(webs[i].id, webs[j].id, 1);
            }
        }
    }
}

/**
 * @brief Computes the effective degree of a vertex within a subset of active vertices.
 * @param v The vertex.
 * @param active Set of currently active vertex IDs.
 * @return Number of neighbors of v that are in the active set.
 */
static int getEffectiveDegree(Vertex<int>* v, const std::unordered_set<int>& active) {
    int degree = 0;
    for (auto* e : v->getAdj()) {
        if (active.count(e->getDest()->getInfo())) {
            degree++;
        }
    }
    return degree;
}

/**
 * @brief Implementation of GraphColoring::basicColoring.
 * @see GraphColoring.h for full documentation.
 */
bool GraphColoring::basicColoring(Graph<int>& ig, int K,
                                  std::unordered_map<int, int>& colorAssignment) {
    std::unordered_set<int> active;
    for (auto* v : ig.getVertexSet()) {
        active.insert(v->getInfo());
    }

    std::stack<int> nodeStack;

    while (!active.empty()) {
        bool found = false;
        for (auto it = active.begin(); it != active.end();) {
            Vertex<int>* v = ig.findVertex(*it);
            int deg = getEffectiveDegree(v, active);
            if (deg < K) {
                nodeStack.push(*it);
                it = active.erase(it);
                found = true;
            } else {
                ++it;
            }
        }
        if (!found) {
            return false;
        }
    }

    while (!nodeStack.empty()) {
        int id = nodeStack.top();
        nodeStack.pop();

        std::vector<bool> used(K, false);
        Vertex<int>* v = ig.findVertex(id);
        for (auto* e : v->getAdj()) {
            int nid = e->getDest()->getInfo();
            auto it = colorAssignment.find(nid);
            if (it != colorAssignment.end() && it->second >= 0 && it->second < K) {
                used[it->second] = true;
            }
        }

        int color = -1;
        for (int c = 0; c < K; c++) {
            if (!used[c]) {
                color = c;
                break;
            }
        }
        if (color == -1) {
            return false;
        }
        colorAssignment[id] = color;
    }

    return true;
}

/**
 * @brief Implementation of GraphColoring::coloringWithSpilling.
 * @see GraphColoring.h for full documentation.
 */
bool GraphColoring::coloringWithSpilling(Graph<int>& ig, int K,
                                          std::unordered_map<int, int>& colorAssignment,
                                          std::vector<int>& spilledWebs,
                                          int maxSpills) {
    std::unordered_set<int> active;
    for (auto* v : ig.getVertexSet()) {
        active.insert(v->getInfo());
    }

    int effectiveMax = (maxSpills < 0) ? ig.getVertexSet().size() : maxSpills;

    while (spilledWebs.size() < static_cast<size_t>(effectiveMax) &&
           spilledWebs.size() < ig.getVertexSet().size()) {
        colorAssignment.clear();
        std::unordered_set<int> remaining = active;
        std::stack<int> nodeStack;

        while (!remaining.empty()) {
            bool found = false;
            for (auto it = remaining.begin(); it != remaining.end();) {
                Vertex<int>* v = ig.findVertex(*it);
                int deg = getEffectiveDegree(v, remaining);
                if (deg < K) {
                    nodeStack.push(*it);
                    it = remaining.erase(it);
                    found = true;
                } else {
                    ++it;
                }
            }
            if (!found) break;
        }

        if (remaining.empty()) {
            while (!nodeStack.empty()) {
                int id = nodeStack.top();
                nodeStack.pop();

                std::vector<bool> used(K, false);
                Vertex<int>* v = ig.findVertex(id);
                for (auto* e : v->getAdj()) {
                    int nid = e->getDest()->getInfo();
                    auto it = colorAssignment.find(nid);
                    if (it != colorAssignment.end() && it->second >= 0 && it->second < K) {
                        used[it->second] = true;
                    }
                }

                int color = -1;
                for (int c = 0; c < K; c++) {
                    if (!used[c]) {
                        color = c;
                        break;
                    }
                }
                if (color == -1) {
                    return false;
                }
                colorAssignment[id] = color;
            }
            return true;
        }

        int worstId = -1;
        int worstDeg = -1;
        for (int id : remaining) {
            Vertex<int>* v = ig.findVertex(id);
            int deg = getEffectiveDegree(v, remaining);
            if (deg > worstDeg) {
                worstDeg = deg;
                worstId = id;
            }
        }

        if (worstId == -1) break;

        spilledWebs.push_back(worstId);
        active.erase(worstId);
    }

    return false;
}

/**
 * @brief Implementation of GraphColoring::allocateRegistersFree.
 * @see GraphColoring.h for full documentation.
 */
template <class T>
std::map<T, std::string> GraphColoring::allocateRegistersFree(Graph<T>* graph, int maxRegisters) {
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

template std::map<int, std::string> GraphColoring::allocateRegistersFree(Graph<int>* graph, int maxRegisters);

/**
 * @brief Selects the web with the highest degree in the interference graph.
 * @param graph The interference graph.
 * @return The vertex ID of the highest-degree web, or -1 if the graph is empty.
 */
static int pickWebToSplit(const Graph<int>& graph) {
    const auto& vertices = graph.getVertexSet();
    if (vertices.empty()) return -1;
    int worstId = vertices.front()->getInfo();
    size_t maxDeg = vertices.front()->getAdj().size();
    for (auto* v : vertices) {
        if (v->getAdj().size() > maxDeg) {
            maxDeg = v->getAdj().size();
            worstId = v->getInfo();
        }
    }
    return worstId;
}

/**
 * @brief Splits a web's live range at its midpoint.
 * @details The original web keeps intervals and program points at or before the
 * split point. A new web (with incremented ID and label suffixed with "_s")
 * receives the remainder. Intervals spanning the split point are divided.
 * @param webs The vector of all webs (the new web is appended).
 * @param idx Index in @p webs of the web to split.
 * @param nextId The next available web ID (incremented after creating the new web).
 * @param entry Output tuple populated as (originalId, newWebId, splitPoint).
 */
static void splitWeb(std::vector<Web>& webs, size_t idx, int& nextId,
                     std::tuple<int,int,int>& entry) {
    Web& original = webs[idx];

    int splitPoint = -1;
    if (original.liveRanges.size() > 1) {
        for (size_t i = 0; i + 1 < original.liveRanges.size(); ++i) {
            if (original.liveRanges[i].end + 1 < original.liveRanges[i + 1].start) {
                splitPoint = original.liveRanges[i].end;
                break;
            }
        }
    }
    if (splitPoint < 0) {
        int minStart = original.liveRanges.front().start;
        int maxEnd = original.liveRanges.front().end;
        for (const auto& interval : original.liveRanges) {
            if (interval.start < minStart) minStart = interval.start;
            if (interval.end > maxEnd) maxEnd = interval.end;
        }
        splitPoint = (minStart + maxEnd) / 2;
    }

    Web newWeb;
    newWeb.id = nextId++;
    newWeb.label = original.label + "_s" + std::to_string(nextId);

    std::vector<Interval> originalRanges, newRanges;
    for (const auto& interval : original.liveRanges) {
        if (interval.end <= splitPoint) {
            originalRanges.push_back(interval);
        } else if (interval.start > splitPoint) {
            newRanges.push_back(interval);
        } else {
            originalRanges.push_back({interval.start, splitPoint});
            newRanges.push_back({splitPoint + 1, interval.end});
        }
    }

    std::vector<ProgramPoint> originalPoints, newPoints;
    for (const auto& point : original.points) {
        if (point.line <= splitPoint) {
            originalPoints.push_back(point);
        } else {
            newPoints.push_back(point);
        }
    }

    original.liveRanges = originalRanges;
    original.points = originalPoints;
    newWeb.liveRanges = newRanges;
    newWeb.points = newPoints;

    entry = std::make_tuple(original.id, newWeb.id, splitPoint);
    webs.push_back(newWeb);
}

/**
 * @brief Implementation of GraphColoring::coloringWithSplitting.
 * @see GraphColoring.h for full documentation.
 */
bool GraphColoring::coloringWithSplitting(std::vector<Web>& webs, int K, int maxSplits,
                                           std::unordered_map<int, int>& colorAssignment,
                                           std::vector<std::tuple<int,int,int>>& splitLog) {
    int nextId = 0;
    for (const auto& w : webs)
        nextId = std::max(nextId, w.id + 1);

    int splitsUsed = 0;

    while (true) {
        InterferenceGraph ig(K);
        ig.build(webs);

        colorAssignment.clear();
        bool ok = basicColoring(ig.getGraph(), K, colorAssignment);

        if (ok) {
            if (splitsUsed > 0)
                std::cout << "[Splitting] Coloring succeeded after "
                          << splitsUsed << " split(s).\n";
            return true;
        }

        if (splitsUsed >= maxSplits) {
            std::cout << "[Splitting] Budget exhausted (" << maxSplits
                      << " split(s) used). Coloring still fails.\n";
            return false;
        }

        int targetId = pickWebToSplit(ig.getGraph());
        if (targetId == -1) return false;

        size_t idx = 0;
        for (size_t i = 0; i < webs.size(); ++i)
            if (webs[i].id == targetId) { idx = i; break; }

        std::cout << "[Splitting] Split " << (splitsUsed + 1) << "/" << maxSplits
                  << ": web " << targetId << " ('" << webs[idx].label
                  << "', degree=" << ig.getGraph().findVertex(targetId)->getAdj().size()
                  << ")\n";

        std::tuple<int,int,int> entry;
        splitWeb(webs, idx, nextId, entry);
        splitLog.push_back(entry);
        ++splitsUsed;
    }
}