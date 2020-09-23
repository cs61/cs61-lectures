#include <cstring>
#include <cstdlib>

int fun(const char* s) {
    return strtol(s, nullptr, 0) + 1;
}
