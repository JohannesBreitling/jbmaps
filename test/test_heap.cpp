

#include "../src/datastructures/heaps/basic_heap.hpp"
#include "../src/utils/constants.hpp"

int main() {
    
    heap::MinIDHeap heap(24);

    heap.insert({4, 4});
    heap.insert({9, 9});
    heap.insert({12, 12});
    heap.insert({7, 7});
    heap.insert({20, 20});
    heap.decreaseKey({12, 10});
    heap.insert({1, 1});
    heap.insert({3, 3});
    heap.insert({6, 6});
    heap.insert({11, 11});
    heap.insert({13, 13});
    heap.insert({16, 16});
    heap.insert({22, 22});

    heap.print();
    uint32_t lastMin = 0;
    while (!heap.empty()) {
        const auto min = heap.extractMin().key;
        assert(min >= lastMin);
        lastMin = min;
        heap.print();
    }
    
}