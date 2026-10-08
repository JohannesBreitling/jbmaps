
#include <stdio.h>
#include <vector>

#include "./datastructures/graph/attributes/lat.hpp"
#include "./datastructures/graph/attributes/long.hpp"
#include "./datastructures/labels/vertex_pair.hpp"

#include "./utils/graph_util.hpp"

struct RoutingBackendConfig {
    bool path;
    bool bidirectional;
    bool searchSpace;
};

template <typename GraphT>
class RoutingBackendFileVisualization {

public:
    RoutingBackendFileVisualization(const GraphT &inputGraph_) : inputGraph(inputGraph_) {}

    void visualize() const {
        invokeTurtle();
        generatePdf();
    }

    void setConfig(RoutingBackendConfig newConfig) {
        conf = newConfig;
    }

    void processPath(const std::vector<uint32_t> &path) const {
        std::vector<float> startLats;
        std::vector<float> startLons;

        std::vector<float> endLats;
        std::vector<float> endLons;

        for (uint32_t i = 1; i < path.size(); i++) {
            startLats.push_back(inputGraph.template getVertexAttr<graph::LatAttribute>(path[i - 1]));
            startLons.push_back(inputGraph.template getVertexAttr<graph::LongAttribute>(path[i - 1]));
            endLats.push_back(inputGraph.template getVertexAttr<graph::LatAttribute>(path[i]));
            endLons.push_back(inputGraph.template getVertexAttr<graph::LongAttribute>(path[i]));
        }

        write_coords_to_file(startLats, startLons, "./output/temp/path_start.txt");
        write_coords_to_file(endLats, endLons, "./output/temp/path_end.txt");
    }

    void processSearchSpace(std::vector<labels::VertexPair> ss) const {
        std::vector<float> startLats;
        std::vector<float> startLons;

        std::vector<float> endLats;
        std::vector<float> endLons;
        
        for (const auto vp : ss) {
            startLats.push_back(inputGraph.template getVertexAttr<graph::LatAttribute>(vp.from));
            startLons.push_back(inputGraph.template getVertexAttr<graph::LongAttribute>(vp.from));
            endLats.push_back(inputGraph.template getVertexAttr<graph::LatAttribute>(vp.to));
            endLons.push_back(inputGraph.template getVertexAttr<graph::LongAttribute>(vp.to));
        }

        write_coords_to_file(startLats, startLons, "./output/temp/ss_starts.txt");
        write_coords_to_file(endLats, endLons, "./output/temp/ss_ends.txt");
    }

    void processSearchSpace(std::vector<labels::VertexPair> ss, std::string pre) const {
        std::vector<float> startLats;
        std::vector<float> startLons;

        std::vector<float> endLats;
        std::vector<float> endLons;
        
        for (const auto vp : ss) {
            startLats.push_back(inputGraph.template getVertexAttr<graph::LatAttribute>(vp.from));
            startLons.push_back(inputGraph.template getVertexAttr<graph::LongAttribute>(vp.from));
            endLats.push_back(inputGraph.template getVertexAttr<graph::LatAttribute>(vp.to));
            endLons.push_back(inputGraph.template getVertexAttr<graph::LongAttribute>(vp.to));
        }

        write_coords_to_file(startLats, startLons, "./output/temp/" + pre + "_ss_starts.txt");
        write_coords_to_file(endLats, endLons, "./output/temp/" + pre + "_ss_ends.txt");
    }
    
private:
    void invokeTurtle() const {
        std::string cmd = "python3 ./scripts/visualization.py";
        cmd.append(" ./output/karlsruhe/edge_coords_start.txt ./output/karlsruhe/edge_coords_end.txt");

        if (conf.searchSpace && conf.bidirectional) {
            cmd.append(" edges ./output/temp/up_ss_starts.txt ./output/temp/up_ss_ends.txt \\#9dcf95");
            cmd.append(" edges ./output/temp/down_ss_starts.txt ./output/temp/down_ss_ends.txt \\#cfa495");
        } else if (conf.searchSpace) {
            cmd.append(" edges ./output/temp/ss_starts.txt ./output/temp/ss_ends.txt \\#32a6a8");
        }

        if (conf.path)
            cmd.append(" path ./output/temp/path_start.txt ./output/temp/path_end.txt \\#a8327b");
        
        FILE* output = popen(cmd.c_str(), "r");        
        pclose(output);

        // TODO: Error handling
    }

    void generatePdf() const {
        FILE* output = popen("./scripts/convert_ps_to_pdf.sh", "r");
        pclose(output);
        
        // TODO: Error handling
    }

    const GraphT &inputGraph;
    RoutingBackendConfig conf;

};