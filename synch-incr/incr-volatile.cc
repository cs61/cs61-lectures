#include <cstdio>
#include <thread>

void threadfunc(volatile unsigned* x) {
    for (int i = 0; i != 10000000; ++i) {
        *x += 1;
    }
}

int main() {
    std::thread th[4];
    volatile unsigned n = 0;
    for (int i = 0; i != 4; ++i) {
        th[i] = std::thread(threadfunc, &n);
    }
    for (int i = 0; i != 4; ++i) {
        th[i].join();
    }
    printf("%u\n", n);
}
