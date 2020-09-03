#include <cstdio>
#include <cstdlib>
#include "hexdump.hh"

extern "C" {
int add(int a, int b);
}

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        fprintf(stderr, "Usage: add A B\n\
    Prints A + B.\n");
        exit(1);
    }

    int a = strtol(argv[1], 0, 0);
    int b = strtol(argv[2], 0, 0);
    int c = add(a, b);
    printf("%d + %d = %d\n", a, b, c);

    // hexdump_object(a);
    // hexdump_object(b);
    // hexdump_object(c);
}
