
#include <unordered_map>

#include "../src/routing_engine.hpp"
#include "../src/routing_backend_vis.hpp"

#include "../src/datastructures/addresses/address_record.hpp"

#include "../src/datastructures/graph/static_graph.hpp"
#include "../src/datastructures/graph/attributes/lat.hpp"
#include "../src/datastructures/graph/attributes/long.hpp"
#include "../src/datastructures/graph/attributes/osm_node_id.hpp"
#include "../src/datastructures/graph/attributes/travel_time.hpp"
#include "../src/datastructures/graph/attributes/geo_distance.hpp"
#include "../src/datastructures/labels/parent_info_distance_label.hpp"
#include "../src/datastructures/labels/search_space_distance_label.hpp"

#include "../src/algorithms/dijkstra/dijkstra.hpp"
#include "../src/algorithms/dijkstra/bidijkstra.hpp"

#include "../src/utils/command_line_parser.hpp"
#include "../src/utils/vector_util.hpp"

int main() {
    std::vector<Command> commands = {
        {"help", 0},
        {"source-address", 1},
        {"target-address", 2},
        {"route-start", 3},
        {"show-addresses", 4},
        {"clear", 5}
    };

    std::cout << "Read in the addresses and create record....." << std::flush;
    const auto addressList = read_string_list_from_file("./input/karlsruhe/address_list.txt");    
    auto record = address::AddressRecord(addressList);
    std::cout << "done.\n";

    std::cout << "Read the graph data and construct graph....." << std::flush;
    std::vector<uint32_t> firstOut = read_vector_from_file<uint32_t>("./input/karlsruhe/first_out");
    std::vector<uint32_t> heads = read_vector_from_file<uint32_t>("./input/karlsruhe/heads");
    std::vector<uint32_t> travel_times = read_vector_from_file<uint32_t>("./input/karlsruhe/travel_times");
    std::vector<uint32_t> geo_distances = read_vector_from_file<uint32_t>("./input/karlsruhe/geo_distances");

    std::vector<float> lats = read_vector_from_file<float>("./input/karlsruhe/latitudes");
    std::vector<float> longs = read_vector_from_file<float>("./input/karlsruhe/longitudes");
    std::vector<int64_t> node_ids = read_vector_from_file<int64_t>("./input/karlsruhe/osm_node_ids");

    using VertexAttrs = graph::GraphAttributes<graph::LatAttribute, graph::LongAttribute, graph::OsmNodeId>;
    using EdgeAttrs = graph::GraphAttributes<graph::GeoDistanceAttribute>;
    VertexAttrs vertexAttrs(lats, longs, node_ids);
    EdgeAttrs edgeAttrs(geo_distances);

    using GraphT = graph::StaticGraph<VertexAttrs, EdgeAttrs>;
    const GraphT inputGraph(firstOut, heads, vertexAttrs, edgeAttrs);
    const GraphT reverseGraph = inputGraph.reverse();

    std::unordered_map<int64_t, uint32_t> osmIdToVertexIdx;
    for (uint32_t v = 0; v < inputGraph.numVertices(); v++) {
        const auto osmId = inputGraph.template getVertexAttr<graph::OsmNodeId>(v);
        osmIdToVertexIdx[osmId] = v;
    }
    std::cout << "done.\n";

    std::cout << "Construct SSSP Query....." << std::flush;
    using DijkstraT = algos::BidirectionalDijkstraQuery<GraphT, labels::FastResetSearchSpaceDistanceLabel, graph::GeoDistanceAttribute>;
    DijkstraT dijkstraQuery(inputGraph, reverseGraph);
    std::cout << "done.\n";

    std::cout << "Initialize routing engine....." << std::flush;
    using BackendT = RoutingBackendFileVisualization<GraphT>;
    BackendT backend(inputGraph);
    RoutingEngine<DijkstraT, BackendT> engine(osmIdToVertexIdx, dijkstraQuery, backend, record);
    CommandLineParser parser(commands, "jbmaps: ");
    std::cout << "done.\n";

    while (parser.isRunning()) {
        const auto cmdResult = parser.nextCommand();
        Response res;

        if (cmdResult.opcode == -1)
            continue;

        switch (cmdResult.opcode) {
            case 0:
                engine.printUsage();
                break;

            case 1:
                res = engine.setSourceByAddress(cmdResult.parameters);
                break;

            case 2:
                res = engine.setTargetByAddress(cmdResult.parameters);
                break;

            case 3:
                res = engine.SSSP();
                break;

            case 4:
                res = engine.printAddresses();
                break;

            case 5:
                res = {};
                std::cout << "\033[2J\033[1;1H";
        }

        if (res.msg != "")
            std::cout << res.msg << "\n";
    }
}