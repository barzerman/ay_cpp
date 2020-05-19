#include <iostream>
#include <any>
#include <typeindex>
#include <thread>
#include <vector>
#include <unordered_set>
#include <unordered_map>
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


    for(auto &a: th ) {a.join();}
}

namespace {
    using namespace bool_trie;
    struct TriePrinter {
        void operator()(const bool_trie::trie::path_type& path) {
            trie::stream_it(std::cout, path) << std::endl;
        };
    };
    void run_bool_trie() {
        trie t;
        t.add_bool_str("01001");
        t.add_bool_path(1, 0, 0);
        t.traverse_with_path(TriePrinter());
    }
    template <typename ...Args>
    bool char_in(char sample, Args  ...args ) {
        for(auto c: {args...}) {
            if(c == sample)
                return true;
        }
        return false;
    }
}

namespace {
    using namespace std;

    struct Solver {
        using point_id_t = int ;

        using line_id_t = pair<point_id_t , point_id_t>;
        using line_points_t = vector<point_id_t>;
        using points_t = vector<vector<int>>;
        points_t points;

        Solver(vector<vector<int>>& pts) : points(pts) {}

        const points_t::value_type& point(point_id_t p) const {return points[p];}
        line_id_t xy(point_id_t p) const {
            const auto & pt = point(p);
            return {pt[0], pt[1]};
        }

        bool co_linear(point_id_t i, point_id_t j, point_id_t k) const {
            auto [Xi, Yi] = xy(i);
            auto [Xj, Yj] = xy(j);
            auto [Xk, Yk] = xy(k);
            return (Yi-Yk)*(Xi-Xj) == (Xi-Xk)*(Yi-Yj);
        }
        int operator()() {
            auto num_points = points.size();

            vector<unordered_set<point_id_t>> lines;
            lines.reserve(num_points);
            for(point_id_t i=0; i< num_points; i++) {
                lines.clear();
                auto j_starts = i+1;
                for(auto j=j_starts; j< num_points; j++) {
                    point_id_t prior_line = -1;
                    cout << "(" << i << "," << j << ")" << endl;
                    for(point_id_t k = j; k< j; k++) {
                        if(co_linear(i, j, k)) {
                            lines[k].insert(j);
                        }
                    }
                }

            }
            return 0;
        }
    };
    class Solution {
    public:

        int maxPoints(vector<vector<int>>& points) {
            return Solver(points)();
        }
    };
    void test_points() {
        vector<vector<int>> points = {{1,2}, {2,3}, {5,6}};
        cout << "SHIT:";
        Solution().maxPoints(points);
        cout << endl;
    }
}

int main() {
    // std::cout << "SHIT " << char_in('a', 'b', 'a', 'c') << std::endl;
    // run_bool_trie();
    //test_points();
    AbcRun();
    return 0;
}
