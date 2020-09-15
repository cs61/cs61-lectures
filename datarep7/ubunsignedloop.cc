#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "hexdump.hh"
extern void use_int(int);

extern "C" {
long loop(unsigned n1, unsigned n2) {
    long sum = 0;
    for (unsigned i = n1; i <= n2; ++i) {
        sum += i;
        //use_int(i);
    }
    return sum;
}
}

int main(int argc, char** argv) {
    assert(argc >= 3);
    unsigned n1 = strtoul(argv[1], nullptr, 0);
    unsigned n2 = strtoul(argv[2], nullptr, 0);

    printf("Done %u-%u, sum %ld\n", n1, n2, loop(n1, n2));
}
