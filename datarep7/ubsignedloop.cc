#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "hexdump.hh"
extern void use_int(int);

extern "C" {
long loop(int n1, int n2) {
    long sum = 0;
    for (int i = n1; i <= n2; ++i) {
        sum += i;
        //use_int(i);
    }
    return sum;
}
}

int main(int argc, char** argv) {
    assert(argc >= 3);
    int n1 = strtol(argv[1], nullptr, 0);
    int n2 = strtol(argv[2], nullptr, 0);

    printf("Done %d-%d, sum %ld\n", n1, n2, loop(n1, n2));
}
