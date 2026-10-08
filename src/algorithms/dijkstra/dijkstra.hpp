
#include "../../datastructures/heaps/basic_heap.hpp"
#include "../../utils/constants.hpp"

namespace algos {

    // Generic implementation for a dijkstra query
    template <typename GraphT, typename DistanceLabelT, typename WeightT>
    class DijkstraQuery {

    public:

        DijkstraQuery(const GraphT &graph_) : graph(graph_), tentativeDistances(graph.numVertices()), queue(graph.numVertices()) {
            init();    
        }
        
        void init() {
            tentativeDistances.reset();
            queue.clear();
            bestDistance = infty;
        }
    
        void run(const uint32_t s, const uint32_t t) {
            init();
            source = s;
            target = t;
            queue.insert({s, 0});
            tentativeDistances.set(s, 0);

            while (!(queue.empty()) && (queue.peek().id != t) && (queue.peek().key < bestDistance)) {
                relaxVertex(queue.extractMin().id);
            }
        }

        uint32_t getBestDistance() {
            return bestDistance;
        }

        std::vector<uint32_t> getPath() {
            if (!tentativeDistances.supportsParentInfo())
                throw std::runtime_error("Your distance label does not support parent info.");

            auto parent = target;
            std::vector<uint32_t> invPath;
            while (parent != source) {
                invPath.push_back(parent);
                parent = tentativeDistances.getParent(parent);
            }
            invPath.push_back(parent);

            std::reverse(invPath.begin(), invPath.end());
            return invPath;
        }

        std::vector<labels::VertexPair> getSearchSpace() const {
            return tentativeDistances.getSearchSpace();;
        }

    private:
 
        void relaxVertex(const uint32_t v) {
            const auto deg = graph.degree(v);

            for (uint32_t i = 0; i < deg; i++) {
                const auto u = graph.neighbor(v, i);
                const auto weight = graph.template getEdgeAttr<WeightT>(v, i);

                const auto distanceToV = tentativeDistances.get(v);
                const auto distanceToU = tentativeDistances.get(u);
                const auto distanceToUViaV = distanceToV + weight;

                if (distanceToUViaV >= distanceToU)
                    continue;

                if (u == target)
                    bestDistance = distanceToUViaV;

                tentativeDistances.set(u, distanceToUViaV, v);

                if (distanceToU == infty) {
                    queue.insert({u, distanceToUViaV});
                    continue;              
                }

                queue.decreaseKey({u, distanceToUViaV});
            }
        }
 
        const GraphT &graph;

        DistanceLabelT tentativeDistances;
        uint32_t bestDistance = infty;
        uint32_t source = invalid_id;
        uint32_t target = invalid_id;

        heap::MinIDHeap queue;
        
    };

}