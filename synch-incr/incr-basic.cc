#include <cstdio>
#include <thread>

#define NUM_THREADS 4

void threadfunc(unsigned* x) {
    // This is a correct way to increment a shared variable!
    // ... OR IS IT?!?!?!?!?!?!??!?!
    for (int i = 0; i != 10000000; ++i) {
        *x += 1;
    }
}

int main() {
    std::thread th[NUM_THREADS];
    unsigned n = 0;
    for (int i = 0; i != NUM_THREADS; ++i) {
        th[i] = std::thread(threadfunc, &n);
    }
    for (int i = 0; i != NUM_THREADS; ++i) {
        th[i].join();
    }
    printf("%u\n", n);
}
