//
// Created by andrey yanpolsky on 4/24/20.
//
#include "../dma_heap.h"
#include <iostream>


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

int main(int argc, const char* argv[]) {
    {
        dma_heap<int> h1;
        for(auto a: {1, 9, 7, 8}) {
            h1.push_heap(a);
        }
        bfs_print(std::cout, h1);
        for(auto x = h1.dfs_begin(); x.is_valid(); ++x) {
            auto v = *x;
            std::cout << "val:" << v.first << std::endl;
        }
    }
    {

    }

}
