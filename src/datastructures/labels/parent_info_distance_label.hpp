
#include <vector>
#include <numeric>

#include "../../utils/constants.hpp"

#pragma once

namespace labels {
    
    class FastResetParentDistanceLabel {

    public:
        FastResetParentDistanceLabel(const uint32_t n) : distances(n, infty), epochs(n), parents(n, invalid_id), currentEpoch(0) {
            std::iota(parents.begin(), parents.end(), 0);
        }

        void reset() {
            currentEpoch++;
        }
        
        uint32_t get(const uint32_t idx) const {
            return epochs[idx] == currentEpoch ? distances[idx] : infty;
        }

        void set(const uint32_t idx, const uint32_t value) {
            distances[idx] = value;
            epochs[idx] = currentEpoch;
        }

        void set(const uint32_t idx, const uint32_t value, const uint32_t parent) {
            distances[idx] = value;
            parents[idx] = parent;
            epochs[idx] = currentEpoch;
        }

        uint32_t getParent(uint32_t v) const {
            return epochs[v] == currentEpoch ? parents[v] : invalid_id;
        }

        constexpr bool supportsParentInfo() const {
            return true;
        }

        constexpr bool supportsSearchSpace() const {
            return false;
        }

    private:
        std::vector<uint32_t> distances;
        std::vector<uint32_t> epochs;
        std::vector<uint32_t> parents;
        uint32_t currentEpoch;

    };

}