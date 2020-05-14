//
// Created by andrey yanpolsky on 5/13/20.
//

#ifndef CPP_BOOL_TRIE_H
#define CPP_BOOL_TRIE_H

#include <vector>
#include <iostream>

namespace bool_trie {
    class node {
    public:
        node(bool val, bool is_terminal) : val_(val), is_terminal_(is_terminal){}
        using node_ptr_t =  std::unique_ptr<node>::pointer ;

        node& add_child(bool val, bool terminal) {
            auto& child = val ? std::get<1>(child_) : std::get<0>(child_);
            if(!child) {
                child = std::make_unique<node>(val, terminal);
            } else if (terminal && !child->is_terminal_) {
                child->is_terminal_ = true;
            }
            return *child;
        }
        template <int Child>
        const node* get_child() const {
            return std::get<Child>(child_).get();
        }
        void set_terminal() {is_terminal_ = true;}
        bool is_terminal() const {return is_terminal_;}
    private:
        bool is_terminal_;
        bool val_;
        std::pair<std::unique_ptr<node>, std::unique_ptr<node>> child_;
    };

    struct trie {
        node root;

        trie() : root(false, false) {}

        template <typename T>
        static bool char_to_bool(const T& c) {
            if constexpr(std::is_same_v<T, char>) {
                return c == '1';
            } else {
                return bool(c);
            }
        }
        template <typename T>
        node& add_bool_str(const T& bool_str) {
            node* n = &root;
            for(auto const &c: bool_str) {
                n = &(n->add_child(c, false));
            }
            n->set_terminal();
            return *n;
        }
    };
}

#endif //CPP_BOOL_TRIE_H
