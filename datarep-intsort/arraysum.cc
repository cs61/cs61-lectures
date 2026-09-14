#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <unistd.h>

[[noreturn]] void usage(const char* progname);


int main(int argc, char** argv) {
    int n = 10;

    // parse command line arguments
    int optch;
    while ((optch = getopt(argc, argv, "n:")) != -1) {
        if (optch == 'n') {
            n = std::stoi(optarg, nullptr, 0);
        } else {
            usage(argv[0]);
        }
    }
    if (optind != argc) {
        usage(argv[0]);
    }

    // allocate array
    assert(n > 0);
    int* a = new int[n];

    // fill with random numbers between 0 and 1 billion
    {
        std::default_random_engine randomness((std::random_device())());
        std::uniform_int_distribution<int> distribution(0, 1000000000);
        for (int i = 0; i != n; ++i) {
            a[i] = distribution(randomness);
        }
    }

    // get current time
    auto tstamp0 = std::chrono::steady_clock::now();

    // sum array at positions read from input
    size_t sum = 0;
    int bound = 0;
    int input;
    while (fscanf(stdin, "%d", &input) == 1) {
        if (input >= 0 && input < n) {
            sum += a[input];
        } else {
            bound = std::max(bound, input + 1);
        }
    }

    // measure elapsed time
    auto tstamp1 = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::round<std::chrono::microseconds>(tstamp1 - tstamp0);

    // print
    std::cout << std::format("sum:      {}\ntime:     {}\n", sum, elapsed);
    if (bound != 0) {
        std::cout << std::format("*** input out of range, try `-n {}`\n", bound);
    }
    delete[] a;
    return bound == 0 ? 0 : 1;
}


void usage(const char* progname) {
    fprintf(stderr, "Usage: %s [-n SIZE]\n", progname);
    exit(1);
}
