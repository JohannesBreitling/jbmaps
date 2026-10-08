
#include <vector>

#include "../src/datastructures/graph/static_graph.hpp"
#include "../src/datastructures/graph/attributes/osm_node_id.hpp"
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
    std::vector<uint32_t> firstOut = read_vector_from_file<uint32_t>("./input/wart/first_out");
    std::vector<uint32_t> heads = read_vector_from_file<uint32_t>("./input/wart/heads");
    std::vector<uint32_t> weights = read_vector_from_file<uint32_t>("./input/wart/travel_times");

    std::vector<float> lats = read_vector_from_file<float>("./input/wart/latitudes");
    std::vector<float> longs = read_vector_from_file<float>("./input/wart/longitudes");
    std::vector<int64_t> osmNodeIds = read_vector_from_file<int64_t>("./input/wart/osm_node_ids");
    std::cout << "done.\n";

    using VertexAttributes = graph::GraphAttributes<graph::LatAttribute, graph::LongAttribute, graph::OsmNodeId>;
    using EdgeAttributes = graph::GraphAttributes<graph::TravelTimeAttribute>;

    VertexAttributes vertexAttributes(lats, longs, osmNodeIds);
    EdgeAttributes edgeAttributes(weights);

    std::cout << "Build graph....." << std::flush;
    using GraphT = graph::StaticGraph<VertexAttributes, EdgeAttributes>;
    GraphT graph(firstOut, heads, vertexAttributes, edgeAttributes);
    std::cout << "done.\n";

    using DijkstraT = algos::DijkstraQuery<GraphT, labels::FastResetParentDistanceLabel, graph::TravelTimeAttribute>;
    DijkstraT query(graph);

    uint32_t s;
    uint32_t t;
    for (uint32_t v = 0; v < graph.numVertices(); v++) {
        if (graph.template getVertexAttr<graph::OsmNodeId>(v) == 354581109) {
            std::cout << "Found s\n";
            s = v;
        }

        if (graph.template getVertexAttr<graph::OsmNodeId>(v) == 550423011) {
            std::cout << "Found t\n";
            t = v;
        }
    }

    query.run(s, t);
    const auto hubewegUnterdorf = query.getPath();

    std::vector<float> latsPath;
    std::vector<float> lonsPath;
    for (const auto vId : hubewegUnterdorf) {
        latsPath.push_back(graph.template getVertexAttr<graph::LatAttribute>(vId));
        lonsPath.push_back(graph.template getVertexAttr<graph::LongAttribute>(vId));
    }

    write_graph_coords_to_file(graph, "./output/wart");
    write_coords_to_file(latsPath, lonsPath, "./output/wart/hubeweg_unterdorf.txt");
}