#include <iostream>
#include <any>
#include <map>
#include <typeindex>
#include "container/BT.h"

struct Foo {
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>; // not needed as of C++20

    Foo(std::initializer_list<std::variant<int, std::list<int>>> lst) {
        for(auto && a: lst) {

        }
     }
};

namespace test {
    void bin_tree_test() {
        BT<std::string> root(std::string("A"),
                             new BT(std::string("B")),
                             new BT(std::string("C"))
        );
        std::cout << "Tree:" << std::endl << root;
    }

    template <typename T>
    class Moo {

    };

    void std_any_test() {
        std::any x = 120;
        std::cout << (x.has_value() ? "has value" : "no value") <<
        " size " << sizeof(x) <<
        "; value=" << std::any_cast<int>(x) <<
        "; type=" << x.type().hash_code() <<
        std::endl;
    }

    void map_test() {
        std::map<std::string, std::string> xm;

        xm.emplace(
                std::piecewise_construct,
                std::forward_as_tuple("w"),
                std::forward_as_tuple(10, 'w')
                );

        std::tuple<std::string, std::string> tp = {"buck", "moo"};

        xm.emplace(tp);

        xm.emplace(std::tuple<std::string, std::string>("ttt", "ppp"));

        for(const auto& i : xm) {
            std::cout << i.first << " ~ " << i.second << std::endl;
        }
    }
}

namespace any_test {
    class Any {
    public:
        template <typename T>
        explicit Any(T t) {
            p_ = std::unique_ptr<Derived<T>>(new Derived<T>(std::move(t)));
        }

        template <typename T>
        const T* get() const {
            if( const Derived<T>* ptr =  dynamic_cast<Derived<T>*>(p_.get()); ptr )
                return &(ptr->d_);
            else
                return nullptr;
        }
        template <typename T>
        bool print(std::ostream& fp) const {
            if(auto ptr = get<T>(); ptr ) {
                return fp << *ptr, true;
            } else {
                return fp << "", false;
            }
        }
    private:
        struct Base {
            virtual ~Base() = default;
        };

        template <typename T>
        struct Derived : Base {
            T d_;
            explicit Derived(T t) : d_(std::move(t)) {}
        };
        std::unique_ptr<Base> p_;
    };
}

using namespace any_test;
int main() {
    // test::map_test();

    Any a {100};
    if(!a.print<int>(std::cout))
        std::cerr << "<no value> for type:" << typeid(int).name() << std::endl;
    if(!a.print<float>(std::cout))
        std::cerr << "<no value>for type:" << typeid(float).name() << std::endl;

    return 0;
}
