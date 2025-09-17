#include <cstdio>
#include <cctype>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sstream>
#if __linux
#include <sys/syscall.h>
#endif
#include "print_bytes.hh"


[[gnu::noinline]]
unsigned string_hash(const char* s) {
    // copy string into buffer of `unsigned`s
    unsigned ubuf[3];
    memset(ubuf, 0, sizeof(ubuf));
    strcpy(reinterpret_cast<char*>(ubuf), s);
    // compute hash using `unsigned`s
    return (ubuf[0] ^ ubuf[1]) + (ubuf[2] | 1);
}


int main(int argc, char* argv[]) {
    // print hashes of command line arguments
    for (int i = 1; i < argc; ++i) {
        printf("%s hash = %08x\n", argv[i], string_hash(argv[i]));
    }

    // if no command line arguments, print hash of stdin
    if (argc == 1) {
        char buf[BUFSIZ];
        size_t n = fread(buf, 1, sizeof(buf) - 1, stdin);
        // terminate string
        buf[n] = 0;
        printf("<stdin> hash = %08x\n", string_hash(buf));
    }
}


[[maybe_unused]]
void surprise() {
    fprintf(stderr, "This function is never called!\n");

    unsigned seed = 0;
    while (true) {
        fprintf(stderr, "%*s\n", 3 + seed % 48, "DIE");
        static struct timespec wait = { 0, 250'000'000 };
#if __linux
        syscall(SYS_nanosleep, &wait, nullptr);
#endif
        seed = seed * 1664525 + 1013904223;
    }
}
