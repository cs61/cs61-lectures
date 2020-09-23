#include <cstring>

int fun(const char* s) {
    if (strchr(s, '!') != nullptr) {
        return 0; /* fun */
    } else {
        return 1; /* no fun */
    }
}
