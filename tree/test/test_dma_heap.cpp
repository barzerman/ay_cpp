//
// Created by andrey yanpolsky on 4/24/20.
//
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <unordered_map>
#include "../vector_heap.h"
#include "../../test_util.h"

namespace {
    template <typename Heap>
    bool heap_has_pop_invariant(Heap& heap) {
        // empties the heap
        for(auto val = heap.pop(), next_val= heap.pop(); !heap.empty(); val = next_val, next_val = heap.pop()) {
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
        vector_heap<decltype(arr)::value_type> heap;
        for(auto a: arr) heap.push(std::move(a));

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
            std::vector<decltype(heap)::value_type> popped_data;
            popped_data.reserve(arr.size());

            for(; !heap.empty(); ++iter_count) {
                auto x = heap.pop();
                popped_data.push_back(x);
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
                vector_heap<decltype(arr)::value_type> heap;
                for(auto a: arr) heap.push(std::move(a));
                decltype(heap)::value_type iValue = heap.value(i);
                assert(heap.count_value(iValue) == 1);
                heap.erase(i);
                assert(heap.count_value(iValue) == 0);
                assert(heap_has_pop_invariant(heap));
            }
        }
        { // erasing multiples
            auto arr = make_shuffled_array<int>(10000);
            vector_heap<decltype(arr)::value_type> heap;
            for (auto a: arr) heap.push(std::move(a));

            std::vector<decltype(heap)::value_type> erased_data;
            for(int i = 0; i < arr.size(); i += 3 ) {
                erased_data.push_back(heap.value(i));
                heap.erase(i);
            }
            for(auto el = erased_data.begin(); el != erased_data.end(); el++) {
                heap.push(std::move(*el));
            }
            assert(heap_has_pop_invariant(heap));
        }
    }

    namespace tracking {
        template <typename tracker_key, typename heap_data_t, template <typename...> typename kv_t=std::unordered_map, typename size_type=unsigned long>
        struct KVHeapTracker {
            using heap_value_t = std::pair<heap_data_t , tracker_key>;

            struct heap_value_compare {
                bool operator ()(const heap_value_t& l, const heap_value_t& r) const {
                    return l.first < r.first;
                }
            };
            using pos_map_t = kv_t<tracker_key, size_type>;

            void operator()(const heap_value_t& v, size_type pos) {
                if(auto x = pos_map.find(v.second); x != pos_map.end()) {
                    x.second = pos;
                }
            }
            pos_map_t pos_map;
        };
        void test_basic() {
            KVHeapTracker<std::string_view , int> tracker;
            using tracker_t = decltype(tracker);
            vector_heap<tracker_t ::heap_value_t , tracker_t::heap_value_compare, tracker_t> heap;

            //vector_heap<decltype(tracker)::heap_value,
        }
    }
}

int main(int argc, const char* argv[]) {
    test_erase();
    test_basic_interfaces();
    tracking::test_basic();
    return 0;
}
