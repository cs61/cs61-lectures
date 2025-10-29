#include <cstdio>
#include <cstring>
#include <cassert>
#include <unistd.h>

int main(int argc, char* argv[]) {
    FILE* f = stdout;

    if (argc > 1) {
        f = fopen(argv[1], "w");
        assert(f);
    }

    // fork one child
    pid_t p1 = fork();
    assert(p1 >= 0);

    const char* identity;
    if (p1 == 0) {
        identity = "BABY\n";
    } else {
        identity = "mama\n";
    }

    // print process’s identity 1,000,000 times
    for (int i = 0; i != 1000000; ++i) {
        fputs(identity, f);
    }
}
