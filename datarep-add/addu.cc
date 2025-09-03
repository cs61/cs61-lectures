#include <cstdio>
#include <string>
#include "print_bytes.hh"
unsigned add(unsigned a, unsigned b);


int main(int argc, char* argv[]) {
    // we must have exactly 3 arguments (including the program name)
    assert(argc == 3);

    // convert texts to unsigned integers
    unsigned a = std::stoul(argv[1]);
    unsigned b = std::stoul(argv[2]);

    // print their sum
    printf("%u + %u = %u\n", a, b, add(a, b));
}


unsigned add(unsigned a, unsigned b) {
    return a + b;
}
