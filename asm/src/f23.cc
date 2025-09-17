//! -O1
#include <cstdio>

int f(const char* str) {
    if (str) {
        puts(str);
    } else {
        puts("(null)");
    }
    return 0;
}
