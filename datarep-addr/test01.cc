#include <cstdio>
#include <cstdint>

uintptr_t check_addr() {
    int x = 2;
    return reinterpret_cast<uintptr_t>(&x);
}

int main() {
    for (int i = 0; i != 10; ++i) {
        printf("%lu\n", check_addr());
    }
}
