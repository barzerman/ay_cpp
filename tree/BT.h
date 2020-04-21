//
// Created by andrey yanpolsky on 4/20/20.
//

#ifndef CPP_BT_H
#define CPP_BT_H
#include <iostream>
#include <iostream>

template <class T>
class BT {
    T data;
    BT* left, *right;
    static constexpr char pad_char = ' ';

    static std::ostream& _stream_node(std::ostream& fp, const BT* n, int depth = 0)  {
        fp << std::string(4*depth, pad_char) ;
        if(n) {
            fp << n->data << std::endl;
            _stream_node(fp, n->left, depth + 1) ;
            _stream_node(fp, n->right, depth + 1);
        } else {
            fp << "/" << std::endl;
        }
        return fp;
    }
public:
    explicit BT(const T& d, BT* left= nullptr, BT* right=nullptr) : data(d), left(left), right(right) {}

    std::ostream& stream_node(std::ostream& fp) const {
        return _stream_node(fp, this, 0);
    }
    ~BT() {
        delete left;
        delete right;
    }
};

template <class T>
std::ostream& operator <<(std::ostream& fp, const BT<T>& t) {
    return t.stream_node(fp);
}

#endif //CPP_BT_H
