#pragma ide diagnostic ignored "UnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "modernize-use-nodiscard"
//
// Created by andrey yanpolsky on 4/23/20.
//

#ifndef CPP_DMA_HEAP_H
#define CPP_DMA_HEAP_H

#include <functional>
#include <vector>
#include <stack>
#include <queue>
#include <iostream>

// array heap implementation with direct access to elements
// and position tracing. every new element placed in the array
// invokes the callback as its position changes
// * pos 0 - is not part of the heap - the heap starts from 1
// ValueType - the value we store in the heap
// PosTracker - callable used to keep track of the element's position.
//              must have void operator(int) {} defined

using noop = struct {template <typename T> void operator()(T&&, int){};};

inline std::ostream & operator << (std::ostream& fp, const noop& ) {return fp << "<noop>";}

template <typename ValueType, typename Compare=std::less<>, typename PosTracker = noop>
class vector_heap {
public:
    // region Nested Types and Constants
    using tracker_type = PosTracker;
    using value_type = ValueType;
    using vector_type = typename std::vector<value_type>;
    using size_type = typename vector_type::size_type;
    using path_type = std::vector<size_type>; // path in the binary tree 0-root, ... parent(pos), pos
    using children_type = std::pair<size_type, size_type>;
    static const size_type ROOT_POS = 1;

    using const_iterator = typename vector_type::const_iterator;
    // array heap order constant iterators
    // use this iterator for quickly visiting all nodes in the heap - it's much faster than a traversal
    const_iterator begin() const { return vec_.begin()+ROOT_POS;}
    const_iterator end() const { return vec_.end();}

    // endregion
    // region Iterators
    // tree traversal constant iterator
    template <template <typename...> typename Container> class traverse_iterator {
    public:
        explicit traverse_iterator(const vector_heap& h) : heap_(h), cur_pos_(heap_.root()) {
            if(cur_pos_)
                tainer_.push(cur_pos_);
        }
        bool is_valid() const {
            return !tainer_.empty();
        }

        traverse_iterator& operator ++() {
            auto pop_pos = tainer_pop_();
            if(auto [left, right] = heap_.children(pop_pos); left) {
                tainer_.push(left);
                if(right) {
                    tainer_.push(right);
                }
            }
            cur_pos_ = is_valid() ? peek_next_() : 0;
            return *this;
        }
        const vector_heap::value_type& operator *() const {
            return heap_.value(cur_pos_);
        }
        const vector_heap::value_type& operator ->() const {
            return heap_.value(cur_pos_);
        }

    private:
        const vector_heap& heap_;
        size_type cur_pos_;
        using tainer_type = Container<size_type>;
        tainer_type tainer_;
        size_type tainer_pop_() {
            auto val = peek_next_();
            tainer_.pop();
            return val;
        }

        size_type peek_next_() const {
            if constexpr (std::is_same_v<tainer_type, std::stack<size_type>>) {
                return tainer_.top();
            } else if constexpr (std::is_same_v<tainer_type, std::queue<size_type>>) {
                return tainer_.front();
            } else {
                return tainer_.peek();
            }
        }
    };
    using dfs_iterator = traverse_iterator<std::stack>;

    using bfs_iterator = traverse_iterator<std::queue>;
    dfs_iterator dfs_begin() const {return std::move(dfs_iterator (*this));}
    bfs_iterator bfs_begin() const {return bfs_iterator (*this);}

    size_type count_value(const value_type& d) const {
        size_type count = 0;
        scan_values(d, [&](size_type pos) {
            count++;
        });
        return count;
    }
    // scans the heap and invoke the callback `cb` every time value value `v` is encountered
    // callback must have `operator()(const value_type&)` defined
    template <typename CB> void scan_values(const value_type& v, CB cb) const {
        if(empty())
            return;

        for(auto i = vec_.begin() + ROOT_POS; i != vec_.end(); i++) {
            if(*i == v) {
                cb(i-vec_.begin());
            }
        }
    }
    // endregion
    // region Constructors and Assignment operators
    vector_heap() = default;
    vector_heap(const vector_heap&) = default;

    vector_heap(vector_heap&&)  noexcept = default;

    vector_heap& operator =(vector_heap&&)  noexcept = default;
    template <typename Iter> vector_heap(Iter begin, Iter end) {
        for(auto i = begin; i != end; i++ ) {
            push(*i);
        }
    }

    // endregion
    // region Mutating interfaces
    void push(value_type && v) {
        if (vec_.empty())
            vec_.resize(ROOT_POS);

        vec_.emplace_back(v);
        filter_up_();
    }

    // pops the top element
    value_type pop() {
        auto rv = vec_[ROOT_POS];
        if constexpr (!std::is_same_v<tracker_type, noop>) {
            tracker_(vec_.back(), 0);
        }
        vec_[ROOT_POS] = vec_.back();
        vec_.pop_back();
        filter_down_();
        return rv;
    }

    void erase(size_type pos) {
        if(pos == last_pos()) {
            tracker_(vec_.back(), 0);
            vec_.pop_back();
        } else {
            tracker_(vec_[pos], 0);
            vec_[pos] = vec_.back();
            vec_.pop_back();
            tracker_(vec_[pos], pos);
            if(pos < ROOT_POS) {
                filter_down_(pos);
            }
            else {
                if(auto parent_pos = pos/2; is_x_lt_y_(pos, parent_pos)) {
                    filter_up_(pos);
                } else {
                    filter_down_(pos);
                }
            }
        }
    }

    void clear() {
        if constexpr (!std::is_same_v<tracker_type, noop>) {
            for(size_type i = 0; i < vec_.size(); i++) {
                tracker_(vec_[i], 0);
            }
        }
        vec_.clear();
    }

    // endregion
    // region Constant interfaces
    bool empty() const {return vec_.size() <= ROOT_POS;}

    size_type size() const {return vec_.size();}

    size_type last_pos() const {return vec_.size() - ROOT_POS; }

    const value_type& value(size_type pos=ROOT_POS) const {return vec_[pos];}

    children_type children(size_type pos) const {
        if( auto left = 2*pos, right = left +1; left < last_pos()) {
            return  {left, right}; // for most heaps with some elements this is the prevailing branch
        } else if(left < size()) {
            return  {left, 0}; // rare
        } else {
            return {0, 0}; // leaf
        }
    }
    constexpr size_type root() const { return (empty() ? 0: ROOT_POS);}

    const tracker_type & tracker() const {return tracker_;}
    const tracker_type & tracker()  {return tracker_;}
    const auto& get_vec() const {return vec_;}
    // endregion
private:
    bool is_x_lt_y_(size_type x, size_type y) const {
        return Compare()(vec_[x], vec_[y]);
    }
    // if node is less than its parent returns the parents position
    // otherwise returns 0
    size_type is_lt_parent(size_type pos) const {
        auto parent_pos = pos/2;
        if (pos > ROOT_POS) {
            return is_x_lt_y_(pos, parent_pos) ? parent_pos :0;
        } else
            return 0;
    }
    // returns position of the smallest child which is smaller than `pos` node
    // if neither child is smaller than the pos or if there are no children return 0
    size_type is_gt_child(size_type pos) const {
        auto [left, right] = children(pos);
        if(left || right) { // not a leaf
            if(is_x_lt_y_(left, pos)) {
                if(right) { // has right child
                    return  is_x_lt_y_(right, left) ? right : left;
                } else // left is the only child
                    return left;
            } else if(right && is_x_lt_y_(right, pos)) {
                // left child is not smaller than this node but right child exists
                // and is smaller than this node
                return right;
            } else // this is a leaf
                return 0;
        }
        return 0;
    }
    // running up towards the root
    // grabbing the last element by default
    void filter_up_(size_type p = 0) {
        for(size_type pos = (p ? p : last_pos()); pos > ROOT_POS; ) {
            if( auto parent_pos = is_lt_parent(pos)) {
                swap_positions_(pos, parent_pos);
            } else
                break;
        }
    }
    // filter down tries to swap node away from the root
    void filter_down_(size_type p = ROOT_POS) {
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
        tracker_(vec_[x], x);
        tracker_(vec_[y], y);
    }
    std::vector<value_type> vec_;
    tracker_type tracker_;
};

#endif //CPP_DMA_HEAP_H
