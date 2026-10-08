
#include <vector>

#include "../src/datastructures/graph/static_graph.hpp"
#include "../src/datastructures/graph/attributes/lat.hpp"
#include "../src/datastructures/graph/attributes/long.hpp"
#include "../src/datastructures/graph/attributes/travel_time.hpp"
#include "../src/datastructures/labels/parent_info_distance_label.hpp"

#include "../src/algorithms/dijkstra/dijkstra.hpp"
#include "../src/algorithms/dijkstra/bidijkstra.hpp"

#include "../src/utils/vector_util.hpp"
#include "../src/utils/progress_bar.hpp"
#include "../src/utils/graph_util.hpp"

int main() {

    std::cout << "Read graph from file....." << std::flush;
    std::vector<uint32_t> firstOut = read_vector_from_file_off<uint32_t>("./input/karlsruhe_official/first_out");
    std::vector<uint32_t> heads = read_vector_from_file_off<uint32_t>("./input/karlsruhe_official/head");
    std::vector<uint32_t> weights = read_vector_from_file_off<uint32_t>("./input/karlsruhe_official/travel_time");

    std::vector<float> lats = read_vector_from_file_off<float>("./input/karlsruhe_official/latitude");
    std::vector<float> longs = read_vector_from_file_off<float>("./input/karlsruhe_official/longitude");
    std::cout << "done.\n";

    using VertexAttributes = graph::GraphAttributes<graph::LatAttribute, graph::LongAttribute>;
    using EdgeAttributes = graph::GraphAttributes<graph::TravelTimeAttribute>;

    VertexAttributes vertexAttributes(lats, longs);
    EdgeAttributes edgeAttributes(weights);

    std::cout << "Build graph....." << std::flush;
    using GraphT = graph::StaticGraph<VertexAttributes, EdgeAttributes>;
    GraphT graph(firstOut, heads, vertexAttributes, edgeAttributes);
    std::cout << "done.\n";

    write_graph_coords_to_file(graph, "./output/karlsruhe_official");

    return 0;

    std::cout << "Reverse graph....." << std::flush;
    auto revGraph = graph.reverse();
    std::cout << "done.\n";

    using DijkstraT = algos::DijkstraQuery<GraphT, labels::FastResetParentDistanceLabel, graph::TravelTimeAttribute>;
    DijkstraT query(graph);

    

    using BiDijkstraT = algos::BidirectionalDijkstraQuery<GraphT, labels::FastResetParentDistanceLabel, graph::TravelTimeAttribute>;
    BiDijkstraT biQuery(graph, revGraph);

    const auto sources = read_vector_from_file_off<uint32_t>("./input/karlsruhe_official/test/source");
    const auto targets = read_vector_from_file_off<uint32_t>("./input/karlsruhe_official/test/target");
    const auto travel_times = read_vector_from_file_off<uint32_t>("./input/karlsruhe_official/test/travel_time_length");
    const auto instance_size = 100;
    uint32_t correctDijk = 0;
    uint32_t correctBiDijk = 0;

    std::cout << "Run queries.....\n";
    // ProgressBar bar(instance_size, 2, 10);
    for (uint32_t i = 0; i < instance_size; i++) {
        const auto s = sources[i];
        const auto t = targets[i];
        const auto shouldDist = travel_times[i];

        std::cout << s << "->" << t << "\n";

        query.run(s, t);
        biQuery.run(s, t);

        const auto distDijk = query.getBestDistance();
        const auto pathDijk = query.getPath();

        std::cout << "Path Size " << pathDijk.size() << "\n";
        print_vector(pathDijk, [](uint32_t n) {
            return std::to_string(n);
        });

        const auto distBiDijk = biQuery.getBestDistance();
        const auto pathBiDijk = biQuery.getPath();

        std::cout << "Bi Path Size " << pathBiDijk.size() << "\n";
        print_vector(pathBiDijk, [](uint32_t n) {
            return std::to_string(n);
        });

        return 0;

        if (distDijk == shouldDist)
            correctDijk++;
        else 
            std::cout << i << ": should " << shouldDist << " is " << distDijk << "\n";

        // if (distBiDijk == shouldDist)
        //     correctBiDijk++;
        // else 
        //     std::cout << i << ": should " << shouldDist << " is " << distDijk << "\n";

        // bar.advance();
    }
    std::cout << "done.\n";

    std::cout << "Correct Dijkstra: " << correctDijk << " / " << instance_size << "\n";
    // std::cout << "Correct Bidirectional Dijkstra: " << correctBiDijk << " / " << instance_size << "\n";

}