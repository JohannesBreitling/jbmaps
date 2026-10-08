
#include <vector>

#include "../../utils/constants.hpp"

#pragma once

namespace labels {
    
    class BasicDistanceLabel {

    public:
        BasicDistanceLabel(const uint32_t n) : distances(n, infty) {}

        void clear() {
            std::fill(distances.begin(), distances.end(), infty);   
        }

        uint32_t get(const uint32_t idx) const {
            return distances[idx];
        }

        void set(const uint32_t idx, const uint32_t value) {
            distances[idx] = value;
        }

        void set(const uint32_t idx, const uint32_t value, const uint32_t /* parent */) {
            distances[idx] = value;
        }

        constexpr bool supportsParentInfo() {
            return false;
        }

        constexpr bool supportsSearchSpace() {
            return false;
        }

    private:
        std::vector<uint32_t> distances;
    
    };

}