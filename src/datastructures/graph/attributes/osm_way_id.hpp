
#include <vector>
#include <iostream>

#include "../../../utils/vector_util.hpp"
#include "../../../utils/constants.hpp"

#pragma once

namespace graph {

    class OsmWayId {
    public:
        std::string name = "osm_way_id";
        using Type = int64_t;
        
        OsmWayId() : values() {}
    
        void initFromVector(const std::vector<Type> &vec) {
            values = vec;
        }
        
        Type osmWayId(const uint32_t edgeId) const {
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
