
#include <vector>
#include <iostream>

#include "../../../utils/vector_util.hpp"
#include "../../../utils/constants.hpp"

#pragma once

namespace graph {

    class LatAttribute {
    public:
        std::string name = "lat";
        using Type = float;
        
        LatAttribute() : values() {}
    
        void initFromVector(const std::vector<Type> &vec) {
            values = vec;
        }
        
        Type latLong(const uint32_t edgeId) const {
            return values[edgeId];
        }

        void print() const {
            print_vector(values, [](Type p) {
                return std::to_string(p);
            });
        }

        void permute(const std::vector<uint32_t> &perm) {
            std::vector<Type> newValues(values.size());
            for (uint32_t i = 0; i < perm.size(); i++) {
                newValues[i] = values[perm[i]];
            }
            values = newValues;
        }

    protected: 
        std::vector<Type> values;
    };
}
