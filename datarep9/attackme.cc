#include <cstdio>
#include <cctype>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include "hexdump.hh"
extern "C" {

// exec_shell(command)
//   Replace the current process with a process executing a shell command.
void exec_shell(const char* command) {
    const char* args[] = {"/bin/sh", "-c", command, nullptr};
    execv(args[0], (char**) args);
}


// finalize_checksum(buf, u)
//    Compute the finalized checksum for `buf`, given the partial checksum
//    `u`.
[[gnu::noinline]]
unsigned finish_checksum(const char* buf, unsigned u) {
    (void) buf;
    return u ^ 0x61616161;
}


// checksum(str, n)
//    Compute the checksum for `str`, a byte sequence `n` bytes long.
unsigned checksum(const char* str, size_t n) {
    // initialize checksum buffer
    union {
        char c[128];
        unsigned u[32];
    } buf;

    // copy string into checksum buffer
    for (size_t i = 0; i != n; ++i) {
        buf.c[i] = str[i];
    }
    // zero remaining bytes
    for (size_t i = n; i < sizeof(buf); ++i) {
        buf.c[i] = 0;
    }

    // compute initial checksum by interpreting the buffer as `unsigned int`s
    // and adding them up
    unsigned u = 0;
    for (size_t i = 0; i < sizeof(buf) / sizeof(unsigned); ++i) {
        u += buf.u[i];
    }

    // finalize checksum
    return finish_checksum(str, u);
}

}



int main(int argc, char* argv[]) {
    if (argc > 1) {
        // print checksums of command line arguments
        for (int i = 1; i < argc; ++i) {
            size_t len = strlen(argv[i]);
            printf("%s: checksum %08x\n", argv[i], checksum(argv[i], len));
        }
    } else {
        // print checksum of standard input
        static char buf[BUFSIZ];
        size_t n = fread(buf, 1, sizeof(buf), stdin);
        printf("<stdin>: checksum %08x\n", checksum(buf, n));
    }
}
