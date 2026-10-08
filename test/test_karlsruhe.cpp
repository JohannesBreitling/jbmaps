
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
    std::vector<uint32_t> firstOut = read_vector_from_file<uint32_t>("./input/karlsruhe/first_out");
    std::vector<uint32_t> heads = read_vector_from_file<uint32_t>("./input/karlsruhe/heads");
    std::vector<uint32_t> weights = read_vector_from_file<uint32_t>("./input/karlsruhe/travel_times");

    std::vector<float> lats = read_vector_from_file<float>("./input/karlsruhe/latitudes");
    std::vector<float> longs = read_vector_from_file<float>("./input/karlsruhe/longitudes");
    std::cout << "done.\n";

    using VertexAttributes = graph::GraphAttributes<graph::LatAttribute, graph::LongAttribute>;
    using EdgeAttributes = graph::GraphAttributes<graph::TravelTimeAttribute>;

    VertexAttributes vertexAttributes(lats, longs);
    EdgeAttributes edgeAttributes(weights);

    std::cout << "Build graph....." << std::flush;
    using GraphT = graph::StaticGraph<VertexAttributes, EdgeAttributes>;
    GraphT graph(firstOut, heads, vertexAttributes, edgeAttributes);
    std::cout << "done.\n";

    write_graph_coords_to_file(graph, "./output/karlsruhe");
}