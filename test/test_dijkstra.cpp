
#include <vector>

#include "../src/datastructures/graph/static_graph.hpp"
#include "../src/datastructures/graph/attributes/empty_vertex.hpp"
#include "../src/datastructures/graph/attributes/travel_time.hpp"

#include "../src/datastructures/labels/parent_info_distance_label.hpp"

#include "../src/algorithms/dijkstra/dijkstra.hpp"
#include "../src/algorithms/dijkstra/bidijkstra.hpp"

int main() {

    std::vector<uint32_t> firstOut = {0, 2, 4, 6, 7, 9, 9};
    std::vector<uint32_t> heads = {1, 3, 0, 4, 1, 5, 1, 2, 5};
    std::vector<uint32_t> weights = {16, 3, 2, 3, 9, 7, 4, 11, 2};

    using VertexAttributes = graph::GraphAttributes<graph::EmptyAttributeVertex>;
    using EdgeAttributes = graph::GraphAttributes<graph::TravelTimeAttribute>;

    VertexAttributes vertexAttributes;
    EdgeAttributes edgeAttributes(weights);

    using GraphT = graph::StaticGraph<VertexAttributes, EdgeAttributes>;
    GraphT graph(firstOut, heads, vertexAttributes, edgeAttributes);

    std::cout << "graph |V| = " << graph.numVertices() << "\n";

    using DijkstraT = algos::DijkstraQuery<GraphT, labels::FastResetParentDistanceLabel, graph::TravelTimeAttribute>;
    DijkstraT query(graph);

    auto revGraph = graph.reverse();

    using BiDijkstraT = algos::BidirectionalDijkstraQuery<GraphT, labels::FastResetParentDistanceLabel, graph::TravelTimeAttribute>;
    BiDijkstraT biQuery(graph, revGraph);

    query.run(0, 5);
    biQuery.run(0, 5);
    std::cout << "0->5 = " << query.getBestDistance() << "\n";
    std::cout << "bi 0->5 = " << biQuery.getBestDistance() << "\n";

    query.run(1, 2);
    biQuery.run(1, 2);
    std::cout << "1->2 = " << query.getBestDistance() << "\n";
    std::cout << "bi 1->2 = " << biQuery.getBestDistance() << "\n";
    
    query.run(5, 1);
    biQuery.run(5, 1);
    std::cout << "5->1 = " << query.getBestDistance() << "\n";
    std::cout << "bi 5->1 = " << biQuery.getBestDistance() << "\n";
}