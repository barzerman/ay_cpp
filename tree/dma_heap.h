//
// Created by andrey yanpolsky on 4/23/20.
//

#ifndef CPP_DMA_HEAP_H
#define CPP_DMA_HEAP_H

#include <functional>
#include <vector>

// array heap implementation with direct access to elements
// and position tracing. every new element placed in the array
// invokes the callback as its position changes
// * pos 0 - is not part of the heap - the heap starts from 1
// DataType - the data we store in the heap
// PosTracker - callable used to keep track of the element's position.
//              must have void operator(int) {} defined

using identity = struct {void operator()(int){};};
template <typename DataType, typename Compare=std::less<DataType>, typename PosTracker = identity>
class dma_heap {
public:
    using value_type = std::pair<DataType, PosTracker>;
    using vector_type = typename std::vector<value_type>;
    using size_type = typename vector_type::size_type;

    void push_heap(value_type&& v) {
        if (vec_.empty()) {
            vec_.emplace_back(value_type());
        }
        vec_.emplace_back(v);
        filter_down_();
    }
    void push_heap(DataType&& dt, PosTracker pt=identity()) {
        push_heap(value_type {dt, pt});
    }
    // pops the top element
    value_type&& pop_heap() {
        vec_[1] = vec_.pop_back();
        filter_up_();
    }
    value_type&& peek_by_pos(size_type pos) const {
        return vec_[pos];
    }
    [[nodiscard]] bool empty() const {return vec_.size() <= 1;}
    [[nodiscard]] size_type size() const {return vec_.size();}

    typename vector_type::const_iterator heap_begin() const {return vec_.begin() + 1;}
    typename vector_type::const_iterator heap_end() const {return vec_.end();}

private:
    [[nodiscard]] size_type data_x_lt_y_(size_type l, size_type r) const {
        // returns l if l < r. 0 otherwise
        return Compare()(vec_[l].first, vec_[r].first) ? l : 0;
    }
    // if node is less than its parent returns the parents position
    // otherwise returns 0
    [[nodiscard]] size_type is_lt_parent(size_type pos) const {
        return pos > 1 ? data_x_lt_y_(pos, pos/2) : 0;
    }
    // returns position of the smallest child which is smaller than `pos` node
    // if neither child is smaller than the pos or if there are no children return 0
    [[nodiscard]] size_type is_gt_child(size_type pos) const {
        if(pos*2 + 1 < vec_.size()) { // not a leaf
            auto left = pos*2, right = left+1;
            if(data_x_lt_y_(left, pos) || data_x_lt_y_(right, pos)) {
                // returning position of the smallest child smaller than `pos`
                return data_x_lt_y_(right, left) ? right : left;
            }
        }
        return 0;
    }
    // running up towards the root
    // grabbing the last element by default
    void filter_up_(size_type p = 0) {
        for(size_type pos = (p ? p : vec_.size()-1); p > 1; ) {
            if( auto parent_pos = is_lt_parent(pos)) {
                swap_positions_(pos, parent_pos);
            } else
                break;
        }
    }
    // filter down tries to swap node closer to the root
    void filter_down_(size_type p =1) {
        for(auto pos = p; pos < vec_.size();) {
            if(auto child_pos = is_gt_child(pos); child_pos) {
                swap_positions_(pos, child_pos);
            } else
                break;
        }
    }
    void swap_positions_(size_type& x, size_type& y)  {
        std::iter_swap(vec_.begin()+x, vec_.begin()+y);
        std::swap(x, y);
        vec_[x].second(x);
        vec_[y].second(y);
    }
    std::vector<value_type> vec_;
};


#endif //CPP_DMA_HEAP_H
