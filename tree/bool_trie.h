//
// Created by andrey yanpolsky on 5/13/20.
//

#ifndef CPP_BOOL_TRIE_H
#define CPP_BOOL_TRIE_H

#include <vector>
#include <iostream>
#include <sstream>
#include <stack>

namespace bool_trie {
    class node {
    public:
        node(bool val, bool is_terminal) : val_(val), is_terminal_(is_terminal){}
        using node_ptr_t =  std::unique_ptr<node>::pointer ;

        bool value() const {
            return val_;
        }
        char value_char() const {
            return val_ ? '1': '0';
        }

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
        using size_type = unsigned int;
        using path_type = std::vector<const node*>;
        size_type max_path_len;
        node root;
        trie() : root(false, false), max_path_len(1) {}

        static std::ostream& stream_it(std::ostream& fp, const path_type& path)  {
            for(auto const n: path) {
                fp << n->value_char();
            }
            return fp;
        }
        static std::string get_path(const path_type& path) {
            std::ostringstream sstr;
            stream_it(sstr, path);
            return sstr.str();
        }
        template <typename CB>
        void traverse_with_path(CB&& cb) {
            auto traverse_subtree = [&] (const node& s) {
                std::vector<const node*> path;
                path.reserve(max_path_len);

                path.push_back(&s);
                while(!path.empty()) {
                    auto n = path.back();
                    path.pop_back();
                    if(n->is_terminal()) {
                        cb(path);
                    }
                    if(auto left = n->get_child<0>(); left) {
                        path.push_back(left);
                    }
                    if(auto right = n->get_child<1>(); right) {
                        path.push_back(right);
                    }
                }
            };
            if(auto c0 = root.get_child<0>(); c0) {
                traverse_subtree(*c0);
            } else if(auto c1 = root.get_child<1>(); c1) {
                traverse_subtree(*c1);
            }
        }

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
                n = &(n->add_child(char_to_bool(c), false));
            }
            n->set_terminal();
            return *n;
        }
        template <typename ...T>
        node& add_bool_path(T&&... args) {
            node* n = &root;
            size_type length = 0;
            for(auto c: {args...}) {
                n = &(n->add_child(char_to_bool(c), false));
                length++;
            }
            n->set_terminal();
            if(length > max_path_len)
                max_path_len = length;

            return *n;
        }
    };
}

#endif //CPP_BOOL_TRIE_H
