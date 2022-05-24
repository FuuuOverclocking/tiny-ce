#include "entry.hpp"

extern "C" {
    int global_a = 0;
    int global_b = 0;

    void setup_args(int a, int b) {
        global_a = a;
        global_b = b;
    }

    int read_a() {
        return global_a;
    }
    
    int read_b() {
        return global_b;
    }
}