#include <cstdio>
#include <string>
#include <vector>
#include "print_bytes.hh"

int main(int argc, char* argv[]) {
    assert(argc <= 2);

    // first argument is `n`; defaults to 10
    unsigned long n = std::stoul(argc < 2 ? "10" : argv[1]);

    // allocate and initialize enough space for `n` bytes,
    // all containing 61
    char* x = new char[n];
    for (size_t i = 0; i != n; ++i) {
        x[i] = 61;
    }

    print_object(x);

    delete[] x;
}
