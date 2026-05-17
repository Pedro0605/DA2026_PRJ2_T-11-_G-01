// Original code by Gonçalo Leão
// Updated by DA 2024/2025 Team

#ifndef DA_TP_CLASSES_GRAPH
#define DA_TP_CLASSES_GRAPH

#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

/**
 * @file Graph.h
 * @brief Generic directed graph with reverse edges and flow support.
 */

template <class T>
class Edge;

/************************* Vertex  **************************/

template <class T>
/**
 * @brief Graph vertex that stores outgoing/incoming edges and traversal state.
 * @tparam T Vertex payload type.
 */
class Vertex {
public:
    /** @brief Constructs a vertex with payload value. */
    Vertex(T in);

    /** @brief Returns vertex payload. */
    T getInfo() const;
    /** @brief Returns outgoing adjacency list. */
    const std::vector<Edge<T> *>& getAdj() const;
    /** @brief Returns visitation flag used by traversals. */
    bool isVisited() const;
    /** @brief Returns predecessor edge used in path reconstruction. */
    Edge<T> *getPath() const;

    /** @brief Updates vertex payload. */
    void setInfo(T info);
    /** @brief Sets visitation state. */
    void setVisited(bool visited);
    /** @brief Sets predecessor edge pointer. */
    void setPath(Edge<T> *path);
    /** @brief Adds outgoing edge to destination with weight/capacity. */
    Edge<T> * addEdge(Vertex<T> *dest, double w);
    /** @brief Removes outgoing edges targeting payload @p in. */
    bool removeEdge(T in);
    /** @brief Removes all outgoing edges. */
    void removeOutgoingEdges();
protected:
    T info;                // info node
    std::vector<Edge<T> *> adj;  // outgoing edges

    // auxiliary fields
    bool visited = false; // used by DFS, BFS, Prim ...
    Edge<T> *path = nullptr;

    std::vector<Edge<T> *> incoming; // incoming edges

    void deleteEdge(Edge<T> *edge);
};

/********************** Edge  ****************************/

template <class T>
/**
 * @brief Directed graph edge with reverse-edge and flow metadata.
 * @tparam T Vertex payload type.
 */
class Edge {
public:
    /** @brief Constructs an edge between origin and destination vertices. */
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w);

    /** @brief Returns destination vertex. */
    Vertex<T> * getDest() const;
    /** @brief Returns edge weight/capacity. */
    double getWeight() const;
    /** @brief Returns origin vertex. */
    Vertex<T> * getOrig() const;
    /** @brief Returns reverse edge pointer. */
    Edge<T> *getReverse() const;
    /** @brief Returns current flow value. */
    double getFlow() const;

    /** @brief Sets reverse edge pointer. */
    void setReverse(Edge<T> *reverse);
    /** @brief Sets current flow value. */
    void setFlow(double flow);

    /** @brief Updates edge weight/capacity. */
    void setWeight(double w) { this->weight = w; }
protected:
    Vertex<T> * dest; // destination vertex
    double weight; // edge weight, can also be used for capacity

    // used for bidirectional edges
    Vertex<T> *orig;
    Edge<T> *reverse = nullptr;

    double flow; // for flow-related problems
};

/********************** Graph  ****************************/

template <class T>
/**
 * @brief Generic directed graph with utilities used by flow algorithms.
 * @tparam T Vertex payload type.
 */
class Graph {
public:
    /** @brief Releases all vertices and edges. */
    ~Graph();
    /*
    * Auxiliary function to find a vertex with a given the content.
    */
    /**
     * @brief Finds a vertex by payload.
     * @param in Payload value to search.
     * @return Pointer to vertex or nullptr if absent.
     */
    Vertex<T> *findVertex(const T &in) const;
    /*
     * Adds a vertex with a given content or info (in) to a graph (this).
     * Returns true if successful, and false if a vertex with that content already exists.
     */
    /**
     * @brief Adds a vertex if it does not already exist.
     * @param in Vertex payload value.
     * @return True when inserted, false when duplicate.
     */
    bool addVertex(const T &in);
    /**
     * @brief Removes a vertex and incident edges.
     * @param in Payload value to remove.
     * @return True when removed, false when missing.
     */
    bool removeVertex(const T &in);

    /*
     * Adds an edge to a graph (this), given the contents of the source and
     * destination vertices and the edge weight (w).
     * Returns true if successful, and false if the source or destination vertex does not exist.
     */
    /**
     * @brief Adds a flow-capacity edge and its reverse residual edge.
     * @param sourc Source vertex payload.
     * @param dest Destination vertex payload.
     * @param w Capacity/weight of forward edge.
     * @return True on success, false if source or destination does not exist.
     */
    bool addFlowEdge(const T &sourc, const T &dest, double w);

    /** @brief Returns graph vertex set. */
    std::vector<Vertex<T> *> getVertexSet() const;

    /** @brief Resets all edge flows to zero. */
    void resetFlows() {
        for (auto* v : vertexSet) {
            for (auto* e : v->getAdj()) {
                e->setFlow(0);
            }
        }
    }

protected:
    std::vector<Vertex<T> *> vertexSet;    // vertex set

};


/************************* Vertex  **************************/

template <class T>
Vertex<T>::Vertex(T in): info(in) {}
/*
 * Auxiliary function to add an outgoing edge to a vertex (this),
 * with a given destination vertex (d) and edge weight (w).
 */
template <class T>
Edge<T> * Vertex<T>::addEdge(Vertex<T> *d, double w) {
    auto newEdge = new Edge<T>(this, d, w);
    adj.push_back(newEdge);
    d->incoming.push_back(newEdge);
    return newEdge;
}

/*
 * Auxiliary function to remove an outgoing edge (with a given destination (d))
 * from a vertex (this).
 * Returns true if successful, and false if such edge does not exist.
 */
template <class T>
bool Vertex<T>::removeEdge(T in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        Vertex<T> *dest = edge->getDest();
        if (dest->getInfo() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true; // allows for multiple edges to connect the same pair of vertices (multigraph)
        }
        else {
            it++;
        }
    }
    return removedEdge;
}

/*
 * Auxiliary function to remove an outgoing edge of a vertex.
 */
template <class T>
void Vertex<T>::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

template <class T>
T Vertex<T>::getInfo() const {
    return this->info;
}

template <class T>
const std::vector<Edge<T>*>& Vertex<T>::getAdj() const {
    return this->adj;
}

template <class T>
bool Vertex<T>::isVisited() const {
    return this->visited;
}

template <class T>
Edge<T> *Vertex<T>::getPath() const {
    return this->path;
}

template <class T>
void Vertex<T>::setInfo(T in) {
    this->info = in;
}

template <class T>
void Vertex<T>::setVisited(bool visited) {
    this->visited = visited;
}

template <class T>
void Vertex<T>::setPath(Edge<T> *path) {
    this->path = path;
}

template <class T>
void Vertex<T>::deleteEdge(Edge<T> *edge) {
    Vertex<T> *dest = edge->getDest();
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig() == this) {
            it = dest->incoming.erase(it);
        }
        else {
            it++;
        }
    }
    delete edge;
}

/********************** Edge  ****************************/

template <class T>
Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w): orig(orig), dest(dest), weight(w), flow(0) {}

template <class T>
Vertex<T> * Edge<T>::getDest() const {
    return this->dest;
}

template <class T>
double Edge<T>::getWeight() const {
    return this->weight;
}

template <class T>
Vertex<T> * Edge<T>::getOrig() const {
    return this->orig;
}

template <class T>
Edge<T> *Edge<T>::getReverse() const {
    return this->reverse;
}

template <class T>
double Edge<T>::getFlow() const {
    return flow;
}

template <class T>
void Edge<T>::setReverse(Edge<T> *reverse) {
    this->reverse = reverse;
}

template <class T>
void Edge<T>::setFlow(double flow) {
    this->flow = flow;
}

/********************** Graph  ****************************/

template <class T>
std::vector<Vertex<T> *> Graph<T>::getVertexSet() const {
    return vertexSet;
}

/*
 * Auxiliary function to find a vertex with a given content.
 */
template <class T>
Vertex<T> * Graph<T>::findVertex(const T &in) const {
    for (auto v : vertexSet)
        if (v->getInfo() == in)
            return v;
    return nullptr;
}

/*
 * Adds a vertex with a given content or info (in) to a graph (this).
 * Returns true if successful, and false if a vertex with that content already exists.
 */
template <class T>
bool Graph<T>::addVertex(const T &in) {
    if (findVertex(in) != nullptr)
        return false;
    vertexSet.push_back(new Vertex<T>(in));
    return true;
}

/*
 * Removes a vertex with a given content (in) from a graph (this), and
 * all outgoing and incoming edges.
 * Returns true if successful, and false if such vertex does not exist.
 */
template <class T>
bool Graph<T>::removeVertex(const T &in) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        if ((*it)->getInfo() == in) {
            auto v = *it;
            v->removeOutgoingEdges();
            for (auto u : vertexSet) {
                u->removeEdge(v->getInfo());
            }
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}

template <class T>
bool Graph<T>::addFlowEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, 0); 
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

template <class T>
Graph<T>::~Graph() {
    for (auto* v : vertexSet) {
        if (v != nullptr)
            v->removeOutgoingEdges();
    }
    for (auto* v : vertexSet) {
        if (v != nullptr) {
            delete v;
        }
    }
}

#endif /* DA_TP_CLASSES_GRAPH */