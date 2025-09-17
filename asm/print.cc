//! -O2 -fcf-protection=none
#include <unistd.h>
#include <cstdio>
#include <cstring>

int main() {
    // Disable output buffering to simplify trace
    setvbuf(stdout, nullptr, _IONBF, 0);

    fputs("Hello, 61 world\n", stdout);
}
