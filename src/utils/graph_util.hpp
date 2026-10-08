
#include <vector>
#include <fstream>

#include "../datastructures/graph/static_graph.hpp"
#include "../datastructures/graph/attributes/lat.hpp"
#include "../datastructures/graph/attributes/long.hpp"

#pragma once

template <typename GraphT>
void write_graph_coords_to_file(const GraphT &graph, const std::string path) {
    std::ofstream outputFileVertex(path + "/vertex_coords.txt");
    std::ofstream oEdgesStart(path + "/edge_coords_start.txt");
    std::ofstream oEdgesEnd(path + "/edge_coords_end.txt");

    for (uint32_t v = 0; v < graph.numVertices(); v++) {
        const auto lat = graph.template getVertexAttr<graph::LatAttribute>(v);
        const auto lon = graph.template getVertexAttr<graph::LongAttribute>(v);

        outputFileVertex << std::to_string(lat) << "," << std::to_string(lon) << "\n";

        const auto deg = graph.degree(v);
        for (uint32_t i = 0; i < deg; i++) {
            const auto u = graph.neighbor(v, i);
            
            const auto lat2 = graph.template getVertexAttr<graph::LatAttribute>(u);
            const auto lon2 = graph.template getVertexAttr<graph::LongAttribute>(u);

            oEdgesStart << std::to_string(lat) << "," << std::to_string(lon) << "\n";
            oEdgesEnd << std::to_string(lat2) << "," << std::to_string(lon2) << "\n";
        }
    }

    outputFileVertex.close();
    oEdgesStart.close();
    oEdgesStart.close();
}

void write_coords_to_file(const std::vector<float> &pathLats, const std::vector<float> &pathLongs, const std::string filePath) {
    std::ofstream outputFilePath(filePath);

    for (uint32_t i = 0; i < pathLats.size(); i++) {
        const auto lat = pathLats[i];
        const auto lon = pathLongs[i];
        outputFilePath << std::to_string(lat) << "," << std::to_string(lon) << "\n";
    }

    outputFilePath.close();
}

void write_coords_to_file_prefix(const std::vector<float> &pathLats, const std::vector<float> &pathLongs, const std::vector<std::string> prefixes, const std::string filePath) {
    std::ofstream outputFilePath(filePath);

    for (uint32_t i = 0; i < pathLats.size(); i++) {
        const auto lat = pathLats[i];
        const auto lon = pathLongs[i];
        outputFilePath << prefixes[i] << std::to_string(lat) << "," << std::to_string(lon) << "\n";
    }

    outputFilePath.close();
}