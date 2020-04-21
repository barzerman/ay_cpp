#include <iostream>
#include "tree/BT.h"

int main() {
    BT<std::string> root(std::string("A"),
            new BT(std::string("B")),
            new BT(std::string("C"))
            );
    std::cerr << "Tree:" << std::endl << root;
    return 0;
}
