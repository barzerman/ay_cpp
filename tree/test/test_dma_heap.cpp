//
// Created by andrey yanpolsky on 4/24/20.
//
#include <iostream>
#include <algorithm>
#include <assert.h>
#include "../dma_heap.h"
#include "../../test_util.h"

namespace {
    template <typename Heap>
    bool heap_has_pop_invariant(Heap& heap) {
        // empties the heap
        for(auto val = heap.pop().first, next_val= heap.pop().first; !heap.empty(); val = next_val, next_val = heap.pop().first) {
            if(!(val <= next_val)) {
                std::cerr << "heap invariant violated:" << val << "," << next_val << std::endl;
                return false;
            }
        }
        return true;
    }
}

using namespace test_util;
namespace {
    void test_basic_interfaces() {
        auto arr = make_shuffled_array<int>(100);
        dma_heap<decltype(arr)::value_type> heap;
        for(auto a: arr) heap.push(a);

        // region Testing traversals
        { // dfs traversal
            auto iter_count = 0;
            for (auto x = heap.dfs_begin(); x.is_valid(); ++x, ++iter_count);
            assert(iter_count == arr.size());
        }
        { // bfs traversal
            auto iter_count = 0;
            for (auto x = heap.bfs_begin(); x.is_valid(); ++x, ++iter_count);
            assert(iter_count == arr.size());
        }
        { // array order
            auto iter_count = 0;
            for (auto a: heap) { ++iter_count; }
            assert(iter_count == arr.size());
        }
        // endregion
        { // testing pop
            auto iter_count = 0;
            std::vector<decltype(heap)::data_type> popped_data;
            popped_data.reserve(arr.size());

            for(; !heap.empty(); ++iter_count) {
                auto x = heap.pop();
                popped_data.push_back(x.first);
            }
            assert(iter_count == arr.size());
            assert(heap.empty());

            // confirming the invariant - popped_data is ordered
            for(auto x = popped_data.begin(), y = x+1; y != popped_data.end(); x=y, y=x+1) {
                assert(*x < *y);
            }
        }
    }
    void test_erase() {

        for(int x =0; x < 10; x++) { // testing erasing single elements
            auto arr = make_shuffled_array<int>(100);
            for(int i=1; i< arr.size(); i++) {
                dma_heap<decltype(arr)::value_type> heap;
                for(auto a: arr) heap.push(a);
                decltype(heap)::data_type iData = heap.data(i);
                assert(heap.count_data(iData) ==1);
                heap.erase(i);
                assert(heap.count_data(iData) ==0);
                assert(heap_has_pop_invariant(heap));
            }
        }
        { // erasing multiples
            auto arr = make_shuffled_array<int>(10000);
            dma_heap<decltype(arr)::value_type> heap;
            for (auto a: arr) heap.push(a);

            std::vector<decltype(heap)::data_type> erased_data;
            for(int i = 0; i < arr.size(); i += 3 ) {
                erased_data.push_back(heap.data(i));
                heap.erase(i);
            }
            for(auto el = erased_data.begin(); el != erased_data.end(); el++) {
                heap.push(*el);
            }
            assert(heap_has_pop_invariant(heap));
        }
    }
}

int main(int argc, const char* argv[]) {
    //test_broken();
    test_erase();
    test_basic_interfaces();
    return 0;
}
