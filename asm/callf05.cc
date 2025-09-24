//! -O2
#include <cstdio>

struct message {
    int n;
    const char* str;
};

void f(message m) {
    for (int i = 0; i < m.n; ++i) {
        puts(m.str);
    }
}
