//
// Created by andrey yanpolsky on 4/30/20.
//

#ifndef CPP_TEST_UTIL_H
#define CPP_TEST_UTIL_H
#include <algorithm>
#include <random>
#include <sstream>

namespace test_util {
    template<typename Iter>
    std::ostream &stream_range(std::ostream &fp, Iter begin, Iter end, const char* delim = " ") {
        return std::copy(begin, end,
                         std::ostream_iterator<typename std::iterator_traits<Iter>::value_type>(fp, delim)), fp;
    }

    template<typename Iter>
    std::string join(Iter begin, Iter end, const char* delim = ", ") {
        std::ostringstream sstr;

        stream_range(sstr, begin, end, delim);
        return sstr.str();
    }
    template <typename Vec>
    void random_shuffle_vec(Vec& v) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(v.begin(), v.end(), g);
    }

    template <typename T>
    auto make_shuffled_array(int size)
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::vector<T> v(size);
        std::iota(v.begin(), v.end(), 0);
        std::shuffle(v.begin(), v.end(), g);
        return v;
    }
}
#endif //CPP_TEST_UTIL_H
