#pragma once

#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

namespace graph {

    /** 
     * Struct for edge permutation
     */
    struct Edge {
        uint32_t tail;
        uint32_t head;
        uint32_t originalIndex;
    };
    
    /**
     * Collection of attributes for vertices and edges of the graph
     */
    template <typename ...Attributes>
    class GraphAttributes : public Attributes... {
    public:
        GraphAttributes() : Attributes()... {}

        template <typename ...Vecs>
        GraphAttributes(const Vecs& ...vecs) : Attributes()... {
            initFromVectors<Attributes...>(vecs...);
        }

        void applyPermutation(const std::vector<uint32_t> &perm) {
            (Attributes::permute(perm), ...);
        }

        void print() const {
            (Attributes::print(), ...);
        }

        GraphAttributes get() const {
            return *this;
        }
    
    private:
        template<typename Attr, typename ...AttrRest, typename Vec, typename ...VecRest>
        void initFromVectors(const Vec &vec, const VecRest& ...rest) {
            Attr::initFromVector(vec);
            initFromVectors<AttrRest...>(rest...);
        }

        template<typename ...AttrRest>
        void initFromVectors() {}
    
    };

    /**
     * Class for a static graph that is weighted and directed
     * Edges and Vertices have a collection of attributes
     */
    template <typename VertexAttributes, typename EdgeAttributes>
    class StaticGraph : public VertexAttributes, public EdgeAttributes {
    public:
        StaticGraph(const std::vector<uint32_t> &firstOut_, const std::vector<uint32_t> &head_, VertexAttributes vertexAttributes_, EdgeAttributes edgeAttributes_) 
          : VertexAttributes(vertexAttributes_),
            EdgeAttributes(edgeAttributes_),
            num_vertices(firstOut_.size() - 1), num_edges(head_.size()),
            firstOut(firstOut_),
            head(head_) {}

        // Returns the degree of a vertex
        uint32_t degree(const uint32_t v) const {
            assert(v < num_vertices);
            return firstOut[v + 1] - firstOut[v];
        }

        // Returns the id of the ith neighbor
        uint32_t neighbor(const uint32_t v, const uint32_t i) const {
            assert(i < degree(v));
            return head[firstOut[v] + i];
        }

        void permuteEdges(const std::vector<uint32_t> &perm) {            
            // edgeAttributes.applyPermutation(perm);
            EdgeAttributes::applyPermutation(perm);
        }

        void permuteVertices(const std::vector<uint32_t> &perm, const std::vector<uint32_t> &reversePerm) {
            // Permute the vertex attributes
            VertexAttributes::applyPermutation(perm);

            // Permute the actual edges
            std::vector<Edge> edges;
            for (unsigned i = 0; i < num_vertices; i++) {
                const auto deg = degree(i);
                const auto offset = firstOut[i];
                for (unsigned j = 0; j < deg; j++) {
                    Edge e;    
                    e.tail = reversePerm[i];
                    e.head = reversePerm[head[offset + j]];
                    e.originalIndex = edges.size();
                    edges.push_back(e);
                }
            }

            // Edges are not sorted anymore
            assert(edges.size() == num_edges);
            std::stable_sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
                return e1.tail < e2.tail;
            });

            // Refill firstOut and weights
            std::vector<unsigned> newFirstOut;
            std::vector<unsigned> newHead;
            std::vector<unsigned> edgePerm;
            
            newFirstOut.push_back(0);
            
            unsigned lastTail = 0;
            unsigned currentEdges = 0;

            assert(num_edges == edges.size());

            for (const auto e : edges) {
                edgePerm.push_back(e.originalIndex);
                while (lastTail != e.tail) {
                    newFirstOut.push_back(currentEdges);
                    lastTail++;
                }
                newHead.push_back(e.head);
                currentEdges++;
            }

            while (lastTail < num_vertices - 1) {
                newFirstOut.push_back(currentEdges);
                lastTail++;
            }

            newFirstOut.push_back(num_edges);
            
            firstOut = newFirstOut;
            head = newHead;
            permuteEdges(edgePerm);
        }

        void permuteVertices(const std::vector<uint32_t> &perm) {
            // Input: mapping order->vertex (e.g perm[0] = 3 means the the first vertex that should be contracted is vertex 3)
            // So you need to map 3->0
            std::vector<uint32_t> reversePerm(perm.size());
            for (uint32_t i = 0; i < perm.size(); i++) {
                reversePerm[perm[i]] = i;
            }

            permuteVertices(perm, reversePerm);
        }

        uint32_t numVertices() const {
            return num_vertices;   
        }

        uint32_t numEdges() const {
            return num_edges;
        }

        StaticGraph reverse() const {
            std::vector<Edge> edges;
            for (unsigned i = 0; i < num_vertices; i++) {
                const auto deg = degree(i);
                const auto offset = firstOut[i];
                for (unsigned j = 0; j < deg; j++) {
                    Edge e;
                    e.tail = i;
                    e.head = head[offset + j];
                    e.originalIndex = edges.size();
                    edges.push_back(e);
                }
            }

            assert(edges.size() == num_edges);

            std::stable_sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
                return e1.head < e2.head;
            });

            // Refill firstOut and weights
            std::vector<unsigned> newFirstOut;
            std::vector<unsigned> permutation;
            std::vector<unsigned> newHead;
            
            newFirstOut.push_back(0);
            
            unsigned lastHead = 0;
            unsigned currentEdges = 0;

            assert(num_edges == edges.size());

            // Edges are sorted by head, the heads will be the tails for the reverse graph
            for (const auto e : edges) {
                permutation.push_back(e.originalIndex);
                while (lastHead != e.head) {
                    newFirstOut.push_back(currentEdges);
                    lastHead++;
                }
                newHead.push_back(e.tail);
                currentEdges++;
            }

            while (lastHead < num_vertices - 1) {
                newFirstOut.push_back(currentEdges);
                lastHead++;
            }

            newFirstOut.push_back(num_edges);

            assert(newFirstOut.size() == num_vertices + 1);
            assert(num_edges == currentEdges);
            assert(newFirstOut.size() == firstOut.size());
            assert(newHead.size() == head.size());

            // Construct new Graph
            // Construct new parameter packs
            StaticGraph reverseGraph(newFirstOut, newHead, VertexAttributes::get(), EdgeAttributes::get());
            reverseGraph.permuteEdges(permutation);

            return reverseGraph;
        }

        template <typename Attr>
        const typename Attr::Type &getEdgeAttr(const uint32_t v, const uint32_t i) const {
            assert(v < num_vertices);
            assert(i < degree(v));
            
            const auto base = firstOut[v];
            const auto idx = base + i;

            return Attr::values[idx];
        }

        template <typename Attr>
        const typename Attr::Type &getEdgeAttr(const uint32_t idx) const {
            assert(idx < num_edges);   
            return Attr::values[idx];
        }

        template <typename Attr>
        const typename Attr::Type &getVertexAttr(const uint32_t v) const {
            assert(v < num_vertices);
            return Attr::values[v];
        }

        void print() {
            std::cout << "Printing graph......\n";
            std::cout << "Vertices from 0.." << (num_vertices - 1) << "\n";
            for (uint32_t v = 0; v < num_vertices; v++) {
                std::cout << "N(" << v << ") = {";
                std::string sep = "";
                auto fo = firstOut[v];
                for (uint32_t i = 0; i < degree(v); i++) {
                    std::cout << sep;
                    std::cout << head[fo + i];
                    sep = ", ";
                }
                std::cout << "}\n";
            }

            std::cout << "Vertex Attributes: \n";
            VertexAttributes::print();

            std::cout << "Edge Attributes: \n";
            EdgeAttributes::print();

            std::cout << "done.\n";
        }

        bool isFullyConnected() const {
            return isFullyConnected(reverse());
        }

        bool isFullyConnected(const StaticGraph<VertexAttributes, EdgeAttributes> &reverseGraph) const {
            std::vector<bool> markedVertices(num_vertices);

            std::vector<uint32_t> vertexQueue;
            vertexQueue.push_back(0);

            while (vertexQueue.size() > 0) {
                const auto v = vertexQueue[vertexQueue.size() - 1];
                markedVertices[v] = true;
                vertexQueue.pop_back();

                // Relax vertex
                const auto deg = degree(v);
                for (uint32_t i = 0; i < deg; i++) {
                    const auto neigh = neighbor(v, i);
                    if (!markedVertices[neigh])
                        vertexQueue.push_back(neigh);
                }

                const auto revDeg = reverseGraph.degree(v);
                for (uint32_t i = 0; i < revDeg; i++) {
                    const auto neigh = reverseGraph.neighbor(v, i);
                    if (!markedVertices[neigh])
                        vertexQueue.push_back(neigh);
                }
            }

            // Check the vertices that are not marked
            // while (true) {
            //     for (const auto isMarked : markedVertices) {
            //         if (!isMarked) {
            //             std::vector<uint32_t> reachedInRun;
            //             std::vector<bool> markedInRun(num_vertices, false);

            //             while (vertexQueue.size() > 0) {
            //                 const auto v = vertexQueue[vertexQueue.size() - 1];
                            
            //                 if 
                            
            //                 reachedInRun.push_back(v);
            //                 const auto deg = degree(v);
            //                 for (uint32_t i = 0; i < deg; i++) {
            //                     const auto neigh = neighbor(v, i);
            //                     if (!markedInRun[neigh])
            //                         vertexQueue.push_back(neigh);
            //                 }
            //             }
            //         }
            //     }
            // }
            
            // Check if all vertices are reached
            uint32_t reached = 0;
            for (const auto isMarked : markedVertices) {
                if (isMarked)
                    reached++;
            }

            std::cout << "Reached " << reached << " / " << num_vertices << "\n";

            return reached == num_vertices;
        }

        void checkDegreeDistribution() const {
            uint32_t maxDegree = 0;
            uint32_t maxDegreeNode = 0;
            std::vector<uint32_t> degs(1);
            for (uint32_t v = 0; v < num_vertices; v++) {
                const auto deg = degree(v);
                if (deg > maxDegree) {
                    degs.resize(deg + 1);
                    maxDegree = deg;
                    maxDegreeNode = v;
                }

                degs[deg]++;
            }

            print_vector(degs, [](const uint32_t &n) {
                return std::to_string(n);
            });
        }
    
    private:

        const uint32_t num_vertices;
        const uint32_t num_edges;

        std::vector<uint32_t> firstOut;
        std::vector<uint32_t> head;

    };

}
