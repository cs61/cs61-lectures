#ifndef CS61_SPINLOCK_FAIRNESS_HH
#define CS61_SPINLOCK_FAIRNESS_HH
#include <cstddef>
#include <cstdio>
#include <cassert>
#include <atomic>

struct runcounters {
    static constexpr size_t max = 40;
    unsigned long c[max + 1];
    unsigned long total = 0;
    unsigned long max_run = 0;

    runcounters() {
        for (size_t i = 0; i != max + 1; ++i) {
            c[i] = 0;
        }
    }

    void add(size_t n) {
        ++c[n < max ? n : max];
        total += n;
        if (n > max_run) {
            max_run = n;
        }
    }
    void add(const runcounters& x) {
        for (size_t i = 0; i != max + 1; ++i) {
            c[i] += x.c[i];
        }
        total += x.total;
        if (x.max_run > max_run) {
            max_run = x.max_run;
        }
    }

    void print() {
        for (size_t i = 0; i != max; i += 4) {
            printf("%4zu: %12lu %12lu %12lu %12lu\n", i, c[i], c[i + 1], c[i + 2], c[i + 3]);
        }
        printf("%3zu+: %12lu\n", max, c[max]);
        printf("total: %11lu\nmax_run: %9lu\n", total, max_run);
    }
};

template <typename T>
void fairness_threadfunc(T* lock, size_t nrounds, unsigned long* id, runcounters* ct) {
    size_t run = 0, expected = 0, i;

    for (i = 0; i != nrounds; ++i) {
        lock->lock();
        std::atomic_thread_fence(std::memory_order_acquire);

        unsigned long current = *id;
        if (current != expected && run != 0) {
            ct->add(run);
            run = 0;
        }
        ++run;
        *id = expected = current + 1;

        std::atomic_thread_fence(std::memory_order_release);
        lock->unlock();
    }

    ct->add(run);
}

#endif
