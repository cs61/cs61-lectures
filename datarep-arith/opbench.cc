#include <cstdio>
#include <cassert>
#include <string>

int main(int argc, char* argv[]) {
    assert(argc == 2 || argc == 3);
    unsigned long b = std::stoul(argv[1]);
    unsigned long n = argc == 2 ? 1'000'000'000 : std::stoul(argv[2]);

    volatile unsigned long sum = 0;  // `volatile` inhibits optimizations
    for (unsigned long a = 0; a != n; ++a) {
        sum += a % b;
    }

    printf("%lu\n", sum);
}
