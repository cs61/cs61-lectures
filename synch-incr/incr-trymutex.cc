#include <cstdio>
#include <thread>
#include <mutex>

#define NUM_THREADS 4

std::mutex mutex;

void threadfunc(unsigned* x) {
    for (int i = 0; i != 10000000; ++i) {
        while (!mutex.try_lock()) {
        }
        *x += 1;
        mutex.unlock();
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
