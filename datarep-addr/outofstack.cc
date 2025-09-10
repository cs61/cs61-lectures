#include <cstdio>
#include <string>
#include "print_bytes.hh"

[[gnu::noinline]] /* inhibit optimizations we don’t want */
void recurse(int a, int b) {
    int c = a + b;

    printf("Running f(%d, %d)...\n", a, b);
    print_object(a);
    print_object(b);
    print_object(c);

    if (b > 0) {
        recurse(a + 1, b - 1);
    }
}

int main(int argc, char* argv[]) {
    assert(argc <= 3);
#if SAN
    printf("\n* This program produces surprising results when compiled with sanitizers!\n\n");
#endif

    // Arguments are `a` and `b`; default to 1 and 2
    int a = std::stoi(argc < 2 ? "1" : argv[1]);
    int b = std::stoi(argc < 3 ? "2" : argv[2]);

    print_object(a);
    print_object(b);

    recurse(a, b);
}
