#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
        exit(1);
    }

    int x = std::stoi(argv[1], nullptr, 0);
    int x_incr = x + 1;
    assert(x_incr > x);
    printf("%d + 1 == %d\n", x, x_incr);
    printf("%d > %d - assertion passed\n", x_incr, x);
}
