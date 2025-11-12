#include <cstdio>
#include <thread>
#include <atomic>

#define NUM_THREADS 4

void threadfunc(std::atomic<unsigned>* x) {
    for (int i = 0; i != 10000000; ++i) {
        // Increment `*x` by one (not)
        unsigned old_value = *x;
        unsigned new_value = old_value + 1;
        *x = new_value;
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
