
#include <vector>
#include <iostream>

#include "../../../utils/vector_util.hpp"
#include "../../../utils/constants.hpp"

#pragma once

namespace graph {

    class OsmNodeId {
    public:
        std::string name = "osm_node_id";
        using Type = int64_t;
        
        OsmNodeId() : values() {}
    
        void initFromVector(const std::vector<Type> &vec) {
            values = vec;
        }
        
        Type osmNodeId(const uint32_t edgeId) const {
            return values[edgeId];
        }

        void permute(const std::vector<uint32_t> &perm) {
            std::vector<Type> newValues(values.size());
            for (uint32_t i = 0; i < perm.size(); i++) {
                newValues[i] = values[perm[i]];
            }
            values = newValues;
        }

        void print() const {
            print_vector(values, [](Type p) {
                return std::to_string(p);
            });
        }

    protected: 
        std::vector<Type> values;
    };
}
