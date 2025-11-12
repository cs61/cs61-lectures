#include <cstdio>
#include <thread>
#include <atomic>

#define NUM_THREADS 4

void threadfunc(std::atomic<int>* spinlock, unsigned* x) {
    for (int i = 0; i != 10000000; ++i) {
        while (++*spinlock != 1) {
            --*spinlock;
        }

        *x += 1;

        --*spinlock;
    }
}

int main() {
    std::thread th[NUM_THREADS];
    std::atomic<int> spinlock = 0;
    unsigned n = 0;
    for (int i = 0; i != NUM_THREADS; ++i) {
        th[i] = std::thread(threadfunc, &spinlock, &n);
    }
    for (int i = 0; i != NUM_THREADS; ++i) {
        th[i].join();
    }
    printf("%u\n", n);
}
