#include "spinlock-fairness.hh"
#include <thread>
#include <cassert>
#include <atomic>
#include <unistd.h>

struct mylock {
    std::atomic_flag lockval;

    void lock() {
        while (lockval.test_and_set()) {
        }
    }

    void unlock() {
        lockval.clear();
    }
};


constexpr size_t nthreads = 4;
constexpr size_t nrounds = 10'000'000;

int main() {
    std::thread th[nthreads];
    runcounters ctrs[nthreads];

    mylock lock;
    unsigned long id = 0;
    lock.lock();

    for (size_t i = 0; i != nthreads; ++i) {
        th[i] = std::thread(fairness_threadfunc<mylock>, &lock, nrounds, &id, &ctrs[i]);
    }

    lock.unlock();
    for (size_t i = 0; i != nthreads; ++i) {
        th[i].join();
    }
    assert(id == nthreads * nrounds);

    for (size_t i = 1; i != nthreads; ++i) {
        ctrs[0].add(ctrs[i]);
    }
    ctrs[0].print();
}
