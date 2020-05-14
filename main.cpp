#include <iostream>
#include <any>
#include <typeindex>
#include <thread>
#include <vector>
#include "tree/bool_trie.h"


void AbcRun() {
    std::atomic<int> state_ = 0;

    std::vector<std::thread> th;
    th.emplace_back(
        std::thread([&]() {
            for(int i = 0; i< 3; ) {
                auto s = state_.load();
                if(s == 0) {
                    std::cerr << 'A' << std::endl;
                    i++;
                    while(!state_.compare_exchange_weak(s, 1));
                }
            }
        })
    );
    th.emplace_back(
        std::thread([&]() {
            for(int i = 0; i< 3; ) {
                auto s = state_.load();
                if(s == 1) {
                    std::cerr << 'B' <<std::endl;
                    i++;
                    while(!state_.compare_exchange_weak(s, 2));
                }
            }
        })
    );
    th.emplace_back(
            std::thread([&]() {
                for(int i = 0; i< 3; ) {
                    auto s = state_.load();
                    if(s == 2) {
                        std::cerr << 'C' <<std::endl;
                        i++;
                        while(!state_.compare_exchange_weak(s, 0));
                    }
                }
            })
    );


    for(auto &a: th ) {
            a.join();
        }
}

namespace {
    using namespace bool_trie;
    void run_bool_trie() {
        trie t;
        t.add_bool_str("01001");
    }
}

int main() {
    run_bool_trie();
    return 0;
}
