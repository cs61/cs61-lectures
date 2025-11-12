#include <cstdio>
#include <thread>
#include <mutex>

#define NUM_THREADS 4

void threadfunc(std::mutex* m, unsigned* x) {
    for (int i = 0; i != 10000000; ++i) {
        m->lock();
        *x += 1;
        m->unlock();
    }
}

int main() {
    std::thread th[NUM_THREADS];
    std::mutex m;
    unsigned n = 0;
    for (int i = 0; i != NUM_THREADS; ++i) {
        th[i] = std::thread(threadfunc, &m, &n);
    }
    for (int i = 0; i != NUM_THREADS; ++i) {
        th[i].join();
    }
    printf("%u\n", n);
}
