//
// Created by andrey yanpolsky on 5/19/20.
//

#ifndef CPP_CIRCLE_QUEUE_H
#define CPP_CIRCLE_QUEUE_H
#include <vector>
template <typename T>
class circle_queue {
public:
    using vector_type = std::vector<T>;
    using value_type = typename vector_type::value_type;
    using size_type = typename vector_type ::size_type;

    circle_queue(size_type n) : vec_(n+1), beg_(vec_.begin()), end_(vec_.begin()) {}
    circle_queue(vector_type&& vec) {
        vec_.swap(vec);
        beg_ = vec_.begin();
        end_ = beg_;
    }
    void swap(circle_queue& other) noexcept {
        vec_.swap(other.vec_);
        beg_ = other.beg_;
        end_ = other.end_;
    }
    bool empty() const { return beg_== end_; }
    size_type size() const { return beg_ <= end_ ? (end_-beg_) : (vec_.size() -(beg_-end_)); }
    size_type capacity() const {vec_.size()-1;}

    size_type space_left() const {return capacity() - (beg_ <= end_? (end_-beg_) : (beg_-end_)) ; }
    bool is_full()  const {
        return beg_ > end_ ? (beg_-end_ ==1) : (end_-beg_ ==1);
    }
    value_type & front() {return *beg_;}

    const value_type& front() const {return *beg_;}
    const value_type& back() const  {return *end_;}

    void  push_back(const value_type& v) {advance_((*end_ = v, end_));}
    void  emplace_back(value_type&& v) {
        std::swap(*end_, v);
        advance_(end_);
    }
    void  push_front(const value_type& v) {*retreat_(beg_) = v;}
    void  emplace_front(value_type&& v) {std::swap(*retreat_(beg_), v);}

    void  pop_front() {advance_(beg_);}
    void  pop_back()  {retreat_(end_);}

    value_type && unplace_back() {
        if constexpr (std::is_integral_v<T>) {
            T v = back();
            pop_back();
            return std::move(v);
        } else {
            value_type v;
            v.swap(back());
            pop_back();
            return std::move(v);
        }
    }
    value_type && unplace_front() {
        if constexpr (std::is_integral_v<T>) {
            value_type v;
            v.swap(front());
            pop_front();
            return std::move(v);
        } else {
            value_type v;
            v.swap(front());
            pop_front();
            return std::move(v);
        }
    }

private:
    vector_type vec_;
    typename vector_type::iterator beg_, end_;

    typename vector_type::iterator& advance_(typename vector_type::iterator& i) {
        return (i== vec_.end() ? (i = vec_.begin()): ++i);
    }
    typename vector_type::iterator& retreat_(typename vector_type::iterator& i) {
        return i==vec_.begin() ? (i=vec_.begin() + capacity()): --i;
    }
};


#endif //CPP_CIRCLE_QUEUE_H
