
#include <vector>

#include "../../utils/constants.hpp"

#pragma once

namespace labels {
    
    class FastResetDistanceLabel {

    public:
        FastResetDistanceLabel(const uint32_t n) : distances(n, infty), epochs(n), currentEpoch(0) {}

        void reset() {
            currentEpoch++;
        }
        
        uint32_t get(const uint32_t idx) const {
            return epochs[idx] == currentEpoch ? distances[idx] : infty;
        }

        void set(const uint32_t idx, const uint32_t value, const uint32_t /* parent */) {
            distances[idx] = value;
            epochs[idx] = currentEpoch;
        }

        constexpr bool supportsParentInfo() {
            return false;
        }

        constexpr bool supportsSearchSpace() {
            return false;
        }

    private:
        std::vector<uint32_t> distances;
        std::vector<uint32_t> epochs;
        uint32_t currentEpoch;

    };

}