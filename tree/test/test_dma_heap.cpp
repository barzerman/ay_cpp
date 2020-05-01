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
    std::ostream& bfs_print(std::ostream& fp, const Heap& heap, int indent_width=4)  {
        auto depth = heap.size();
        for(; depth > 0; depth /=2);

        int pos = 1;
        std::vector<typename Heap::data_type> stack;

        for(auto i = heap.begin(); i != heap.end(); i++, pos++ ) {
            fp << std::string (indent_width*(pos/2), ' ') << i->first << std::endl;
        }
        return fp;
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
                auto x = heap.pop_data();
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
}

int main(int argc, const char* argv[]) {
    test_basic_interfaces();
    return 0;
}
