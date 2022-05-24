#include "lib.hpp"
#include <iostream>

extern "C" {
    void cgroups_test() {
        std::cout << "cgroups_test" << std::endl;
    }
}