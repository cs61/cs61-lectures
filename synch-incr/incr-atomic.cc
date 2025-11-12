#include <cstdio>
#include <thread>
#include <atomic>

#define NUM_THREADS 4

void threadfunc(std::atomic<unsigned>* x) {
    for (int i = 0; i != 10000000; ++i) {
        *x += 1;
        // `x->fetch_add(1)` and `(*x)++` also work!
    }
}

int main() {
    std::thread th[NUM_THREADS];
    std::atomic<unsigned> n = 0;
    for (int i = 0; i != NUM_THREADS; ++i) {
        th[i] = std::thread(threadfunc, &n);
    }
    for (int i = 0; i != NUM_THREADS; ++i) {
        th[i].join();
    }
    printf("%u\n", n.load());
}
