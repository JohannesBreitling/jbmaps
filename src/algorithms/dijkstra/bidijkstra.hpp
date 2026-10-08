
#include "../../datastructures/heaps/basic_heap.hpp"
#include "../../datastructures/labels/vertex_pair.hpp"

#include "../../utils/constants.hpp"

namespace algos {

    // Generic implementation for a dijkstra query
    template <typename GraphT, typename DistanceLabelT, typename WeightT>
    class BidirectionalDijkstraQuery {

    public:

        BidirectionalDijkstraQuery(const GraphT &forwardGraph_, const GraphT &reverseGraph_) : forwardGraph(forwardGraph_), reverseGraph(reverseGraph_), forwardDistances(forwardGraph_.numVertices()), backwardDistances(forwardGraph_.numVertices()), forwardQueue(forwardGraph_.numVertices()), backwardQueue(forwardGraph_.numVertices()) {
            init();
        }
        
        void init() {
            forwardDistances.reset();
            backwardDistances.reset();
            forwardQueue.clear();
            backwardQueue.clear();
            bestDistance = infty;
            viaVertex = invalid_id;
        }
    
        void run(const uint32_t s, const uint32_t t) {
            init();

            source = s;
            target = t;
            
            forwardQueue.insert({s, 0});
            backwardQueue.insert({t, 0});
            
            forwardDistances.set(s, 0);
            backwardDistances.set(t, 0);

            while (!(forwardQueue.empty() && backwardQueue.empty())) {
                
                if (forwardQueue.empty()) {
                    // Backward queue not empty
                    assert(!backwardQueue.empty());
                    relaxVertex(backwardQueue, reverseGraph, backwardDistances);
                    continue;
                }

                if (backwardQueue.empty()) {
                    // Forward queue not empty
                    assert(!forwardQueue.empty());
                    relaxVertex(forwardQueue, forwardGraph, forwardDistances);
                    continue;
                }
                
                // Both queues are not empty
                const auto forwardKey = forwardQueue.peek().key;
                const auto backwardKey = backwardQueue.peek().key;
                
                if (forwardKey + backwardKey >= bestDistance)
                    return;

                if (backwardKey < forwardKey) {
                    // Advance backward search
                    relaxVertex(backwardQueue, reverseGraph, backwardDistances);
                    continue;
                }
                
                // Advance forward search
                relaxVertex(forwardQueue, forwardGraph, forwardDistances);
            }
        }

        uint32_t getBestDistance() {
            return bestDistance;
        }

        std::vector<uint32_t> getPath() const {
            std::vector<uint32_t> backwardPath;
            uint32_t currentVertex = viaVertex;
            
            while (currentVertex != target) {
                backwardPath.push_back(currentVertex);
                currentVertex = backwardDistances.getParent(currentVertex);
            }
            backwardPath.push_back(currentVertex);
            
            std::vector<uint32_t> invForwardPath;
            currentVertex = forwardDistances.getParent(viaVertex);
            while (currentVertex != source) {
                invForwardPath.push_back(currentVertex);
                currentVertex = forwardDistances.getParent(currentVertex);
            }
            invForwardPath.push_back(currentVertex);
            
            std::reverse(invForwardPath.begin(), invForwardPath.end());
            invForwardPath.insert(invForwardPath.end(), backwardPath.begin(), backwardPath.end());
            
            return invForwardPath;
        }

        std::vector<labels::VertexPair> getSearchSpace() const {
            auto fwdSS = forwardDistances.getSearchSpace();
            auto bwdSS = backwardDistances.getSearchSpace();
            fwdSS.insert(fwdSS.end(), bwdSS.begin(), bwdSS.end());
            
            return fwdSS;
        }

        std::vector<labels::VertexPair> getUpSearchSpace() const {
            return forwardDistances.getSearchSpace();
        }

        std::vector<labels::VertexPair> getDownSearchSpace() const {
            return backwardDistances.getSearchSpace();
        }

    private:

        void relaxVertex(heap::MinIDHeap &queue, const GraphT &graph, DistanceLabelT &distances) {
            const auto p = queue.extractMin();
            const auto v = p.id;
            const auto w = p.key;
            
            const auto deg = graph.degree(v);

            if (deg == 0)
                return;

            for (uint32_t i = 0; i < deg; i++) {
                const auto u = graph.neighbor(v, i);
                // Relax (v, u) edge
                const auto weight = graph.template getEdgeAttr<WeightT>(v, i);
                                
                const auto distanceToV = distances.get(v);
                const auto distanceToU = distances.get(u);
                const auto distanceToUViaV = distanceToV + weight;

                if (distanceToUViaV >= distanceToU)
                    continue;

                distances.set(u, distanceToUViaV, v);

                if (distanceToU == infty) {
                    queue.insert({u, distanceToUViaV});
                    continue;
                }

                queue.decreaseKey({u, distanceToUViaV});
            }

            if (forwardDistances.get(v) + backwardDistances.get(v) < bestDistance) {
                bestDistance = forwardDistances.get(v) + backwardDistances.get(v);
                viaVertex = v;
            }
        }
 
        const GraphT &forwardGraph;
        const GraphT &reverseGraph;

        DistanceLabelT forwardDistances;
        DistanceLabelT backwardDistances;
        
        uint32_t bestDistance = infty;
        uint32_t viaVertex = invalid_id;
        uint32_t source = invalid_id;
        uint32_t target = invalid_id;

        heap::MinIDHeap forwardQueue;
        heap::MinIDHeap backwardQueue;
    };



}