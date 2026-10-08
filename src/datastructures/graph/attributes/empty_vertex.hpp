
#include <vector>

#pragma once

namespace graph {

    class EmptyAttributeVertex {
    public:
        std::string name = "empty_vertex";
        using Type = void;
        
        EmptyAttributeVertex() {}
    
        void initFromVector(const std::vector<uint32_t> &vec) {}
        void permute(const std::vector<uint32_t> &perm) {}

        void print() const {}
    };

}
