#include "GraphColoring.h"
#include <unordered_set>
#include <algorithm>

InterferenceGraph::InterferenceGraph(int K) : K(K) {}

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

bool GraphColoring::coloringWithSpilling(Graph<int>& ig, int K,
                                          std::unordered_map<int, int>& colorAssignment,
                                          std::vector<int>& spilledWebs) {
    std::unordered_set<int> active;
    for (auto* v : ig.getVertexSet()) {
        active.insert(v->getInfo());
    }

    while (spilledWebs.size() < ig.getVertexSet().size()) {
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