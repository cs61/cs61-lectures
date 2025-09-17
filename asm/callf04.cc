//! -O2
#include <cstdio>

void f(int n, const char* str) {
    while (n > 0) {
        puts(str);
        --n;
    }
}
