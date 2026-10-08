
#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <string>

#include "../../utils/constants.hpp"
#include "../../utils/vector_util.hpp"

#pragma once

namespace heap {
    
    struct IdKeyPair {
        uint32_t id;
        uint32_t key;

        IdKeyPair() : id(invalid_id), key(invalid_id) {}
        IdKeyPair(const uint32_t id_, const uint32_t key_) : id(id_), key(key_) {}
    };

    // Implementation of a k-ary tree min heap.
    // The elements are identified by an id that is expected to be between 0 and the max number of elements minus 1.
    class MinIDHeap {

    public:
        MinIDHeap(uint32_t n)
          : max_capacity(n), size(0), heap(n), idToPos(n) {
            std::fill(heap.begin(), heap.end(), IdKeyPair());
            std::fill(idToPos.begin(), idToPos.end(), invalid_id);
        }

        IdKeyPair peek() const {
            assert(size > 0);
            return heap[0];
        }

        IdKeyPair extractMin() {
            assert(size > 0); 
            const auto result = heap[0];
            swap(0, size - 1);
            idToPos[result.id] = invalid_id;
            heap[size - 1] = IdKeyPair();
            --size;
            siftDown(0);

            return result;
        }

        void insert(const IdKeyPair p) {
            // std::cout << "insert " << p.id << " w = " << p.key << "\n";
            assert(size < max_capacity);
            assert(idToPos[p.id] == invalid_id);
            heap[size] = p;
            idToPos[p.id] = size;
            ++size;
            siftUp(size - 1);
        }

        // void increaseKey(const uint32_t id, const uint32_t newKey) {
        //     assert(idToPos[id] != invalid_id);
        //     const auto pos = idToPos[id];
        //     assert(heap[pos].key <= newKey);
        //     heap[pos].key = newKey;
        //     siftDown(pos);
        // }

        void decreaseKey(const IdKeyPair p) {
            // std::cout << "decrease " << id << " w = " << newKey << "\n";
            assert(idToPos[p.id] != invalid_id);
            const auto pos = idToPos[p.id];
            assert(heap[pos].key >= p.key);
            heap[pos].key = p.key;
            siftUp(pos);
        }

        // void deleteElement(const uint32_t id) {
        //     assert(idToPos[id] != invalid_id);
        //     const auto pos = idToPos[id];
        //     assert(pos != invalid_id);
        //     swap(pos, size - 1);
        //     heap[size - 1] = IdKeyPair();
        //     idToPos[size - 1] = invalid_id;
        //     --size;
        //     siftDown(pos);
        // }

        bool empty() {
            return size == 0;    
        }

        void clear() {
            heap.clear();
            std::fill(idToPos.begin(), idToPos.end(), invalid_id);
            size = 0;
        }

        void print() {
            print_vector(heap, [](IdKeyPair p) {
                return "{" + std::to_string(p.id) + ", " + std::to_string(p.key) + "}";
            }, size);
        }

    private:
        inline void swap(uint32_t i, uint32_t j) {
            if (i == j)
                return;
            
            std::swap(idToPos[heap[i].id], idToPos[heap[j].id]);
            std::swap(heap[i], heap[j]);
        }

        void siftUp(uint32_t pos) {
            assert(pos >= 0);
            assert(pos < size);

            if (pos == 0)
                return;
            
            const auto parentPos = pos / HEAP_TREEARITY;
            if (heap[parentPos].key > heap[pos].key) {
                swap(parentPos, pos);
                siftUp(parentPos);
            }
        }

        void siftDown(uint32_t pos) {
            assert(pos >= 0);
            assert(pos < size || size == 0);

            uint32_t minIdx = pos;

            for (uint32_t i = 0; i < HEAP_TREEARITY; i++) {
                const uint32_t childPos = HEAP_TREEARITY * pos + i;
                
                if (childPos >= size)
                    break;
            
                if (heap[childPos].key < heap[pos].key && (heap[childPos].key < heap[minIdx].key)) {
                    minIdx = childPos;
                }
            }

            if (pos != minIdx) {
                swap(pos, minIdx);
                siftDown(minIdx);
            }
        }
    
        const uint32_t max_capacity;
        uint32_t size;

        std::vector<IdKeyPair> heap;
        std::vector<uint32_t> idToPos;
    };

}