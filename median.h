//
// Created by andrey yanpolsky on 5/18/20.
//

#ifndef CPP_MEDIAN_H
#define CPP_MEDIAN_H
#include <vector>

class Solution {
public:
    using vec_t = std::vector<int>;
    using size_t = vec_t::size_type;
    using value_type = vec_t::value_type;
    using val_pos_t = std::tuple<value_type, value_type, size_t>;
    static bool is_odd(size_t n) {return (n&1);}

    static val_pos_t median(const vec_t& v)  {
        size_t pos = 1+v.size()/2;
        if(is_odd(v.size()))
            return {v[pos], v[pos], pos};
        else
            return {v[pos], v[pos+1], pos};
    }
    static auto get_below_count(const vec_t& vec, value_type val) {

    }
    double findMedianSortedArrays(vec_t& nums1, vec_t& nums2) {
        auto [l1, h1, p1] = median(nums1);
        auto [l2, h2, p2] = median(nums2);

        auto h = std::max(h1, h2), l = std::min(l1, l2);
        // the median is between h and l
        size_t below_count, above_count;

        do {

        } while(below_count != above_count);
    }
};
#endif //CPP_MEDIAN_H
