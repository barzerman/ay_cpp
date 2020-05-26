#include <iostream>
#include <any>
#include <typeindex>
#include <thread>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "tree/bool_trie.h"


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

namespace lock_free {
    void AbcRun_old() {
        std::atomic<int> state_ = 0;

        std::vector<std::thread> th;
        th.emplace_back(
                std::thread([&]() {
                    for (int i = 0; i < 3;) {
                        auto s = state_.load();
                        if (s == 0) {
                            std::cerr << 'A' << std::endl;
                            i++;
                            while (!state_.compare_exchange_weak(s, 1));
                        }
                    }
                })
        );
        th.emplace_back(
                std::thread([&]() {
                    for (int i = 0; i < 3;) {
                        auto s = state_.load();
                        if (s == 1) {
                            std::cerr << 'B' << std::endl;
                            i++;
                            while (!state_.compare_exchange_weak(s, 2));
                        }
                    }
                })
        );
        th.emplace_back(
                std::thread([&]() {
                    for (int i = 0; i < 3;) {
                        auto s = state_.load();
                        if (s == 2) {
                            std::cerr << 'C' << std::endl;
                            i++;
                            while (!state_.compare_exchange_weak(s, 0));
                        }
                    }
                })
        );

        for (auto &a: th) { a.join(); }

    }

    class WorkerThread {
        using int_atomic = std::atomic<int>;
        int thread_num_;
        int num_iter_;
        int total_threads_;
        int_atomic & state_;
    public:
        WorkerThread(int_atomic& st, int th_num, int num_it, int total_threads) :
                thread_num_(th_num),
                num_iter_(num_it),
                total_threads_(total_threads),
                state_(st)
        {}
        void operator() () {
            auto next_thread = (thread_num_+1)%total_threads_;
            for (int i = 0; i < num_iter_;) {
                auto s = state_.load();
                if (s == thread_num_) {
                    std::cerr << (char)('A'+thread_num_) << std::endl;
                    i++;
                    while (!state_.compare_exchange_weak(s, next_thread));
                }
            }
        }
    };

    void AbcRun(int num_threads, int num_iter) {
        std::atomic<int> state_ = 0;
        std::vector<std::thread> th;
        for(int i =0; i< num_threads; i++) {
            th.emplace_back(
                    std::thread(
                            WorkerThread(state_, i, num_iter, num_threads)));
        }
        for (auto &a: th) { a.join(); }
    }
}

namespace locks {
    using lock_t = std::mutex;
    struct WorkerThread {
        int thread_id_; // id of this thread
        int num_iter_; // number of iterations
        int max_threads_; // total number of threads
        int &cur_thread_;
        std::condition_variable& cond_;
        lock_t &lock_;

        WorkerThread(lock_t &lock, std::condition_variable& cond, int &cur_thread, int thread_id, int num_iter, int max_threads) :
                thread_id_(thread_id),
                num_iter_(num_iter),
                max_threads_(max_threads),
                cur_thread_(cur_thread),
                cond_(cond),
                lock_(lock)
        {}
        void operator()( ) {
            int i = 0;

            do {
                std::unique_lock ul(lock_);
                while(cur_thread_!= thread_id_) cond_.wait(ul);
                std::cerr << (char)('A'+thread_id_) << std::endl;
                i++;
                cur_thread_ = (cur_thread_+1)%max_threads_;
                cond_.notify_all();

                if(i>= num_iter_)
                    return;
            } while(true);


        }
    };
    void AbcRun(const int num_threads, const int num_iter) {
        std::condition_variable cond;
        std::mutex mtx;
        int cur_thread_= 0;
        std::vector<std::thread> th;
        for(int i =0; i< num_threads; i++) {
            th.emplace_back(
                    std::thread(
                            WorkerThread(mtx, cond, cur_thread_, i, num_iter, num_threads)));
        }
        for (auto &a: th) { a.join(); }
    }

}

int main() {
    //lock_free::AbcRun(4, 10);
    locks::AbcRun(5, 3);
    return 0;
}
