#pragma clang diagnostic push
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

// array heap implementation with direct access to elements
// and position tracing. every new element placed in the array
// invokes the callback as its position changes
// * pos 0 - is not part of the heap - the heap starts from 1
// DataType - the data we store in the heap
// PosTracker - callable used to keep track of the element's position.
//              must have void operator(int) {} defined

using identity = struct {void operator()(int){};};
template <typename DataType, typename Compare=std::less<>, typename PosTracker = identity>
class dma_heap {
public:
    using tracker_type = PosTracker;
    using data_type = DataType;
    using value_type = std::pair<data_type, tracker_type>;
    using vector_type = typename std::vector<value_type>;
    using size_type = typename vector_type::size_type;

    using children_type = std::pair<size_type, size_type>;

    static const size_type ROOT_POS = 1;
    using const_iterator = typename vector_type::const_iterator;

    // constructors and assignment operators
    dma_heap() = default;
    dma_heap(const dma_heap&) = default;
    dma_heap(dma_heap&&)  noexcept = default;
    dma_heap& operator =(dma_heap&&)  noexcept = default;
    // end of constructors and assignment ops

    // array heap order constant iterators
    // use this iterator for quickly visiting all nodes in the heap - it's much faster than a traversal
    const_iterator begin() const { return vec_.begin()+ROOT_POS;}
    const_iterator end() const { return vec_.end();}

    // tree traversal constant iterator
    template <template <typename...> typename Container>
    class traverse_iterator {
    public:
        explicit traverse_iterator(const dma_heap& h) : heap_(h), cur_pos_(heap_.root()) {
            if(cur_pos_)
                tainer_.push(cur_pos_);
        }
        bool is_valid() const {
            return bool(cur_pos_);
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
        const dma_heap::value_type& operator *() const {
            return heap_.value(cur_pos_);
        }
        const dma_heap::value_type& operator ->() const {
            return heap_.value(cur_pos_);
        }
    private:
        const dma_heap& heap_;
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

    template <typename Iter>
    dma_heap(Iter begin, Iter end) {
        for(auto i = begin; i != end; i++ ) {
            push_heap(*i);
        }
    }
    template<typename U = data_type>
    void push_heap(U&& dt, PosTracker&& pt=identity()) {
        static_assert(std::is_same_v<std::decay_t<U>, std::decay_t<DataType>>);

        if (vec_.empty())
            vec_.resize(1);
        vec_.emplace_back(std::move(value_type {dt, pt}));
        filter_up_();
    }

    // pops the top element
    value_type&& pop_heap() {
        vec_[1] = vec_.pop_back();
        filter_down_();
    }
    value_type&& peek_by_pos(size_type pos) const {
        return vec_[pos];
    }
    bool empty() const {return vec_.size() <= 1;}
    size_type size() const {return vec_.size();}

    typename vector_type::const_iterator heap_begin() const {return vec_.begin() + 1;}
    typename vector_type::const_iterator heap_end() const {return vec_.end();}

    size_type last_pos() const {return vec_.size() -1; }

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
private:
    size_type data_x_lt_y_(size_type l, size_type r) const {
        // returns l if l < r. 0 otherwise
        return Compare()(vec_[l].first, vec_[r].first) ? l : 0;
    }
    bool is_x_lt_y_(size_type x, size_type y) const {
        return Compare()(vec_[x].first, vec_[y].first);
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
        vec_[x].second(x);
        vec_[y].second(y);
    }
    std::vector<value_type> vec_;
};

template <typename DmaTree, typename Container=std::stack<typename DmaTree::size_type>>
struct dma_tree_iterator {
    using size_type = typename DmaTree::size_type;
    using path_type = Container;

    const DmaTree& tree_;

    explicit dma_tree_iterator(const DmaTree& tree): tree_(tree) {}

    template <typename CB=std::unary_function<const path_type& , bool>>
    typename  DmaTree::size_type operator()(CB& cb) const {
        path_type path;
        path.push(tree_.root_pos());
        size_type visited_count = 0;
        while(!path.empty()) {
            auto pos = path.pop();
            visited_count ++;
            if(!cb(pos, path)) {
                break;
            }

            if( auto [left, right] = tree_.children(pos); left ) {
                path.push(left);
                if(right) {
                    path.push(right);
                }
            }
        }
        return visited_count;
    }

};

template <typename DmaTree>
using dma_tree_dfs = dma_tree_iterator<DmaTree, std::stack<typename DmaTree::size_type>>;

template <typename DmaTree>
using dma_tree_bfs = dma_tree_iterator<DmaTree, std::queue<typename DmaTree::size_type>>;

#endif //CPP_DMA_HEAP_H

