#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <random>
#include <string>
#include <unistd.h>

[[noreturn]] void usage(const char* progname);
void ascending_order(int n);
void descending_order(int n);
void random_order(int n);
void random_blocks_order(int n, int block_size = 64);
void hill_order(int n);


int main(int argc, char** argv) {
    int n = 10, order = 'r', block_size = 1;

    // parse command line arguments
    int optch;
    while ((optch = getopt(argc, argv, "rudHB:n:")) != -1) {
        if (optch == 'r' || optch == 'u' || optch == 'd' || optch == 'H') {
            order = optch;
        } else if (optch == 'n') {
            n = std::stoi(optarg, nullptr, 0);
        } else if (optch == 'B') {
            block_size = std::stoi(optarg, nullptr, 0);
        } else {
            usage(argv[0]);
        }
    }
    if (optind != argc) {
        usage(argv[0]);
    }

    // print `n` integers according to `order`
    assert(n > 0 && block_size > 0);
    if (order == 'u') {
        ascending_order(n);
    } else if (order == 'd') {
        descending_order(n);
    } else if (order == 'r') {
        if (block_size == 1) {
            random_order(n);
        } else {
            random_blocks_order(n, block_size);
        }
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


void random_blocks_order(int n, int block_size) {
    std::default_random_engine randomness((std::random_device())());

    // Create an array `v` holding all blocks
    int nblocks = (n + block_size - 1) / block_size;
    int* v = new int[nblocks];
    for (int i = 0; i != nblocks; ++i) {
        v[i] = i * block_size;
    }

    while (nblocks != 0) {
        int idx = uniform_int_between(0, nblocks - 1, randomness),
            block_start = v[idx],
            block_end = std::min(block_start + block_size, n);

        // print the elements of that block
        for (int i = block_start; i != block_end; ++i) {
            printf("%d\n", i);
        }

        // replace that block (which is now printed) with the last unprinted block
        v[idx] = v[nblocks - 1];
        --nblocks;
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


void usage(const char* progname) {
    fprintf(stderr, "Usage: %s [-r|-u|-d|-H] [-n SIZE] [-B BLOCKSIZE]\n", progname);
    exit(1);
}
