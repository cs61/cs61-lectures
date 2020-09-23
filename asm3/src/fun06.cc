#include <cstring>

int fun(const char* s) {
    unsigned len;
    for (len = 0; s[len]; ++len) {
    }
    return len % 4;
}
