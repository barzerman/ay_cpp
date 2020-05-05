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

        void test_kv_tracker() {
            using tracker_t = kv_heap_tracker<int, std::string>;
            vector_heap<tracker_t::heap_value_t , tracker_t::heap_value_compare, tracker_t> heap;

            auto arr = make_shuffled_array<int>(100);
            auto& pos_map = heap.tracker().pos_map;

            std::vector<std::string> names;
            names.reserve(arr.size());

            for(int i=1; i< arr.size(); i++) {
                std::ostringstream sstr;
                sstr << "element" << arr[i];
                names.emplace_back(std::string(sstr.str()));

                const std::string& name = names.back().c_str();
                pos_map[name] = 0;
                heap.push(tracker_t::heap_value_t {arr[i], std::string_view (name)});
            }
            // checking pos consistency
            auto pop_count = 1;
            do {
                for(const auto& a: heap.tracker().pos_map) {
                    assert(a.first == heap.value(a.second).second);
                }
                auto v = heap.pop();
                pop_count++;
            } while(!heap.empty());
            assert(pop_count == arr.size());
        }
        void test_intrusive_tracker() {
            using tracker_t = intrusive_tracker<int, unsigned long *>;
            vector_heap<tracker_t::heap_value_t, tracker_t::heap_value_compare, tracker_t> heap;

            auto arr = make_shuffled_array<int>(100);
            std::vector<std::pair<int, tracker_t::tracked_position_t>> tracker_vector;
            // need to preallocate the vector so that pointers stay valid as we push_back
            tracker_vector.reserve(arr.size());

            for (int i = 1; i < arr.size(); i++) {
                tracker_vector.emplace_back(decltype(tracker_vector)::value_type{arr[i], 0});
                heap.push(tracker_t::heap_value_t{arr[i], &(tracker_vector.back().second)});
            }
            for(auto const& a: tracker_vector) {
                assert(a.first == heap.value(a.second).first);
            }
            auto pop_count = 0;

            do {
                auto deleted_count = 0;
                for(const auto& a: tracker_vector) {
                    if(a.second)
                     assert(a.first == heap.value(a.second).first);
                    else
                        deleted_count ++;
                }
                assert(pop_count == deleted_count);
                heap.pop();
                pop_count++;
            } while(!heap.empty());

        }
    }
}

int main(int argc, const char* argv[]) {
    tracking::test_intrusive_tracker();
    tracking::test_kv_tracker();
    test_erase();
    test_basic_interfaces();
    return 0;
}
