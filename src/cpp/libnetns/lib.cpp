#include "lib.hpp"
#include <iostream>

extern "C" {
    void netns_test() {
        std::cout << "netns_test" << std::endl;
    }
}