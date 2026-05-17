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
