#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <random>
#include <unistd.h>

void ascending_order(int n);
void descending_order(int n);
void random_order(int n);
void hill_order(int n);


int main(int argc, char** argv) {
    int n = 10, order = 'r';

    // parse command line arguments
    int optch;
    while ((optch = getopt(argc, argv, "rudmHn:")) != -1) {
        if (optch == 'r' || optch == 'u' || optch == 'd' || optch == 'H') {
            order = optch;
        } else if (optch == 'm') {
            order = 'H';
        } else if (optch == 'n') {
            n = strtol(optarg, nullptr, 0);
        } else {
            fprintf(stderr, "Usage: %s [-r|-u|-d|-H] [-n SIZE]\n", argv[0]);
            exit(1);
        }
    }

    // print `n` integers according to `order`
    assert(n > 0);
    if (order == 'u') {
        ascending_order(n);
    } else if (order == 'd') {
        descending_order(n);
    } else if (order == 'r') {
        random_order(n);
    } else {
        hill_order(n);
    }
}


void ascending_order(int n) {
    for (int i = 0; i != n; ++i) {
        fprintf(stdout, "%d\n", i);
    }
}


void descending_order(int n) {
    for (int i = n - 1; i >= 0; --i) {
        fprintf(stdout, "%d\n", i);
    }
}


// Return an integer uniformly distributed between `min` and `max`, inclusive.
template <typename T, typename G>
T uniform_int_between(T min, T max, G& generator) {
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(generator);
}

void random_order(int n) {
    std::default_random_engine randomness((std::random_device())());

    // We want no duplicates, which requires some work!
    // Create an array `v` holding all ints between 0 and `n - 1`.
    int* v = new int[n];
    for (int i = 0; i != n; ++i) {
        v[i] = i;
    }

    while (n != 0) {
        // Invariant: None of the ints in `v[0..n-1]` have been printed.

        // pick the index of an unprinted int
        int idx = uniform_int_between(0, n - 1, randomness);

        // print it
        printf("%d\n", v[idx]);

        // replace that int (which is now printed) with the last unprinted int
        v[idx] = v[n - 1];

        // shrink `n`
        --n;

        // The invariant still holds!
    }

    delete[] v;
}


void hill_order(int n) {
    int z = 0;
    for (int i = 0; i != (n + 1) / 2; ++i) {
        printf("%d\n", z);
        z += 2;
    }
    z -= n % 2 ? 3 : 1;
    for (int i = (n + 1) / 2; i != n; ++i) {
        printf("%d\n", z);
        z -= 2;
    }
}
