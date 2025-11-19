#include <cassert>
#include <random>
#include <thread>
#include <atomic>
#include <mutex>
#include <unistd.h>

struct account {
    long bal = 0;

    void deposit(long amt) {
        bal += amt;
    }

    bool try_withdraw(long amt) {
        if (bal < amt) {
            return false;
        }
        bal -= amt;
        return true;
    }
};


void threadfunc(account* accts, size_t naccts, long* delta) {
    std::default_random_engine randomness((std::random_device())());
    std::uniform_int_distribution<size_t> acctdist(0, naccts - 1);
    std::uniform_int_distribution<long> moneydist(-100, 300);
    long change = 0;

    for (size_t n = 0; n != 10'000'000; ++n) {
        auto a = acctdist(randomness);
        auto m = moneydist(randomness);
        if (m > 0) {
            accts[a].deposit(m);
            change += m;
        } else if (m < 0) {
            if (accts[a].try_withdraw(-m)) {
                change += m;
            }
        }
    }

    *delta = change;
}


constexpr size_t naccts = 10'000;
constexpr size_t nthreads = 4;

int main() {
    account* accts = new account[naccts];

    std::thread th[nthreads];
    long deltas[nthreads];
    for (size_t i = 0; i != nthreads; ++i) {
        th[i] = std::thread(threadfunc, accts, naccts, &deltas[i]);
    }
    for (size_t i = 0; i != nthreads; ++i) {
        th[i].join();
    }

    // check that balance matches
    long atotal = 0, ttotal = 0;
    for (size_t a = 0; a != naccts; ++a) {
        assert(accts[a].bal >= 0);
        atotal += accts[a].bal;
    }
    for (size_t i = 0; i != nthreads; ++i) {
        ttotal += deltas[i];
    }
    assert(atotal == ttotal);
}
