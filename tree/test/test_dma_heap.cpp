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
        dma_heap<int> h2;
        for(auto a: {9, 10, 8, 6, 5, 7, 1}) {
            h2.push_heap(a);
        }
        bfs_print(std::cout, h2);


        dma_tree_dfs<decltype(h1)> traverser(h1);
        [&] (const decltype(traverser)::path_type& path) -> bool {
            std::cout << std::string(4, ' ') << path.peek()
            return true;
        };
    }
    {

    }

}
