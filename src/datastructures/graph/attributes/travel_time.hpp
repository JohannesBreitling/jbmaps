
#include <vector>
#include <iostream>

#include "../../../utils/vector_util.hpp"
#include "../../../utils/constants.hpp"

#pragma once

namespace graph {

    class TravelTimeAttribute {
    public:
        std::string name = "travel_time";
        using Type = uint32_t;
        std::vector<Type> values;
        
        TravelTimeAttribute() : values() {}
    
        void initFromVector(const std::vector<uint32_t> &vec) {
            values = vec;
        }
        
        uint32_t travelTime(const uint32_t edgeId) const {
            return values[edgeId];
        }

        void permute(const std::vector<uint32_t> &perm) {
            std::vector<uint32_t> newValues(values.size());
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
        
    };
}
