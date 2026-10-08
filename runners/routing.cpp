
#include <iostream>

#include "../src/utils/vector_util.hpp"

#include "../src/datastructures/graph/attributes/geo_distance.hpp"
#include "../src/datastructures/graph/attributes/travel_time.hpp"
#include "../src/datastructures/graph/attributes/lat.hpp"
#include "../src/datastructures/graph/attributes/long.hpp"
#include "../src/datastructures/graph/attributes/osm_way_id.hpp"
#include "../src/datastructures/graph/attributes/osm_node_id.hpp"
#include "../src/datastructures/graph/static_graph.hpp"

#include "../src/datastructures/addresses/address_record.hpp"

int main(int argc, char** argv) {
    std::cout << "- - - jbmaps - - -\n";

    if (argc != 2) {
        throw std::runtime_error("Unexpected number of arguments. Expected ./routing <path_to_graph>");
    }

    std::cout << "Read graph data....." << std::flush;
    std::string path = argv[1];
    std::vector<uint32_t> firstOut = read_vector_from_file<uint32_t>(path + "/first_out");
    std::vector<uint32_t> heads = read_vector_from_file<uint32_t>(path + "/heads");
    
    std::vector<uint32_t> geoDistances = read_vector_from_file<uint32_t>(path + "/geo_distances");
    std::vector<uint32_t> travelTimes = read_vector_from_file<uint32_t>(path + "/travel_times");
    
    std::vector<int64_t> osmNodeIds = read_vector_from_file<int64_t>(path + "/osm_node_ids");
    std::vector<int64_t> osmWayIds = read_vector_from_file<int64_t>(path + "/osm_way_ids");

    std::vector<float> lats = read_vector_from_file<float>(path + "/latitudes");
    std::vector<float> longs = read_vector_from_file<float>(path + "/longitudes");

    std::vector<std::string> addressList = read_string_list_from_file(path + "/address_list.txt");
    std::cout << "done.\n";

    std::cout << "Build address dictionary....." << std::flush;
    const address::AddressRecord addressRecord(addressList);
    std::cout << "done.\n";

    std::cout << "Construct graph....." << std::flush;
    using VertexAttributes = graph::GraphAttributes<graph::LatAttribute, graph::LongAttribute, graph::OsmNodeId>;
    using EdgeAttributes = graph::GraphAttributes<graph::TravelTimeAttribute, graph::GeoDistanceAttribute, graph::OsmWayId>;

    const uint32_t num_vertices = firstOut.size() - 1;
    const uint32_t num_edges = heads.size();
    
    VertexAttributes vA(lats, longs, osmNodeIds);
    EdgeAttributes eA(travelTimes, geoDistances, osmWayIds);

    using GraphT = graph::StaticGraph<VertexAttributes, EdgeAttributes>;
    GraphT graph(firstOut, heads, vA, eA);

    GraphT reverseGraph = graph.reverse();
    std::cout << "done.\n";

    return 0;
}