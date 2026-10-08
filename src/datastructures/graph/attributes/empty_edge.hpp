
#include <vector>
#include <iostream>

#include "../../../utils/constants.hpp"

#pragma once

namespace graph {

    class EmptyAttributeEdge {
    public:
        std::string name = "empty_edge";
        using Type = void;
        
        EmptyAttributeEdge() {}
        EmptyAttributeEdge(uint32_t n) {}
    
        void initFromVector(const std::vector<uint32_t> &vec) {}
        void permute(const std::vector<uint32_t> &perm) {}

        void print() const {}
    };

}
