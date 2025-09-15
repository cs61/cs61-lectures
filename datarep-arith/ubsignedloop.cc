#include <cassert>
#include <string>
#include <cstdlib>
#include <cstdio>

[[gnu::noinline]] int f(int x) {
    return x;
}

[[gnu::noinline]] long loop(int n1, int n2) {
    long sum = 0;
    for (int i = n1; i <= n2; ++i) {
        sum += f(i);
    }
    return sum;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s LO HI\n", argv[0]);
        exit(1);
    }

    int n1 = std::stoi(argv[1], nullptr, 0);
    int n2 = std::stoi(argv[2], nullptr, 0);
    printf("Sum of [%d,%d] is %ld\n", n1, n2, loop(n1, n2));
}
