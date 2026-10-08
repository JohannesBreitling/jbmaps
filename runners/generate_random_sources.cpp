
#include <iostream>
#include <random>

#include "../src/utils/vector_util.hpp"
#include "../src/utils/graph_util.hpp"

int main(int argc, char** argv) {
    std::cout << "- - - jbmaps - - -\n";

    if (argc != 3) {
        throw std::runtime_error("Unexpected number of arguments. Expected ./generate_random_sources <path_to_graph> <output_path>");
    }

    std::cout << "Read graph data....." << std::flush;
    std::string instance = argv[1];
    std::string output_path = argv[2];
    std::vector<float> lats = read_vector_from_file<float>("./input/" + instance + "/latitudes");
    std::vector<float> lons = read_vector_from_file<float>("./input/" + instance + "/longitudes");
    std::cout << "done\n";

    std::cout << "Init random engine....." << std::flush;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, lats.size());
    std::cout << "done\n";

    const uint32_t num_sources = 40;
    std::cout << "Generate " << num_sources << " random sources....." << std::flush;
    std::vector<float> source_lats;
    std::vector<float> source_lons;
    std::vector<std::string> prefixes;
    for (uint32_t i = 0; i < num_sources; i++) {
        const auto v = dist(gen);
        source_lats.push_back(lats[v]);
        source_lons.push_back(lons[v]);
        prefixes.push_back(std::to_string(v) + ",");
    }

    write_coords_to_file_prefix(source_lats, source_lons, prefixes, output_path);
    std::cout << "done\n";

    return 0;
}