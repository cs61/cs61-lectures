#include "sqmatrix.hh"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cinttypes>
#include <cmath>
#include <ctime>
#include <cassert>
#include <unistd.h>
#include <getopt.h>


// matrix_multiply(c, a, b)
//    `a`, `b`, and `c` are square matrices with dimension `sz`.
//    Computes the matrix product `a * b` and stores it in `c`.
void matrix_multiply(sqmatrix& c, sqmatrix& a, sqmatrix& b) {
    assert(c.size() == a.size() && c.size() == b.size());

    // clear `c`
    for (size_t i = 0; i < c.size(); ++i) {
        for (size_t j = 0; j < c.size(); ++j) {
            c.at(i, j) = 0;
        }
    }

    // transpose `b`
    b.transpose();

    // compute product and update `c`
    for (size_t i = 0; i < c.size(); ++i) {
        for (size_t j = 0; j < c.size(); ++j) {
            for (size_t k = 0; k < c.size(); ++k) {
                c.at(i, j) += a.at(i, k) * b.at(j, k);
            }
        }
    }

    // transpose `b` back
    b.transpose();
}


// sqmatrix_statistics()
//    Compute some statistics about a matrix.
struct sqmatrix_statistics {
    double corner[4];
    double diagonal_sum;

    sqmatrix_statistics(sqmatrix& m);
};

sqmatrix_statistics::sqmatrix_statistics(sqmatrix& m) {
    this->corner[0] = m.at(0, 0);
    this->corner[1] = m.at(0, m.size() - 1);
    this->corner[2] = m.at(m.size() - 1, 0);
    this->corner[3] = m.at(m.size() - 1, m.size() - 1);
    this->diagonal_sum = 0;
    for (size_t i = 0; i != m.size(); ++i) {
        this->diagonal_sum += m.at(i, i);
    }
}



// tstamp()
//    Return current timestamp as a double.
double tstamp() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}


// xrandom()
//    Return a pseudo-random number in the range [0, XRAND_MAX].
//    We use our own generator to ensure values computed on different
//    OSes will follow the same sequence.
#define XRAND_MAX 0x7FFFFFFF
static uint64_t xrandom_seed;
unsigned xrandom() {
    xrandom_seed = xrandom_seed * 6364136223846793005U + 1U;
    return (xrandom_seed >> 32) & XRAND_MAX;
}


int main(int argc, char* argv[]) {
    size_t sz = 1000;
    int has_seed = 0;

    // read options
    int opt;
    while ((opt = getopt(argc, argv, "n:d:")) != -1) {
        switch (opt) {
        case 'n':
            sz = strtoul(optarg, nullptr, 0);
            break;
        case 'd':
            xrandom_seed = strtoul(optarg, nullptr, 0);
            has_seed = 1;
            break;
        default:
            fprintf(stderr, "Usage: ./matrixmultiply [-n SIZE] [-d SEED]\n");
            exit(EXIT_FAILURE);
        }
    }

    assert(sz > 0);
    assert(sz < (size_t) sqrt(SIZE_MAX / sizeof(double)));
    if (!has_seed) {
        union { double d; uint64_t x; } u;
        u.d = tstamp();
        xrandom_seed = u.x;
    }
    printf("size %zu, seed %" PRIu64 "\n", sz, xrandom_seed);

    // allocate matrices
    sqmatrix a(sz);
    sqmatrix b(sz);
    sqmatrix c(sz);

    // fill in source matrices
    for (size_t i = 0; i < sz; ++i) {
        for (size_t j = 0; j < sz; ++j) {
            a.at(i, j) = xrandom() / (double) XRAND_MAX;
        }
    }

    for (size_t i = 0; i < sz; ++i) {
        for (size_t j = 0; j < sz; ++j) {
            b.at(i, j) = xrandom() / (double) XRAND_MAX;
        }
    }

    // compute `c = a * b`
    double t0 = tstamp();
    matrix_multiply(c, a, b);
    double t1 = tstamp();

    // print statistics and differences
    sqmatrix_statistics mstat(c);
    for (int i = 0; i < 4; ++i) {
        printf("corner statistic %d: %g (%a)\n",
               i, mstat.corner[i], mstat.corner[i]);
    }
    printf("diagonal sum statistic: %g (%a)\n",
           mstat.diagonal_sum, mstat.diagonal_sum);

    // print times
    if (isatty(STDOUT_FILENO)) {
        printf("\x1b[1mmultiply time %.09f\x1b[m\n", t1 - t0);
    } else {
        printf("multiply time %.09f\n", t1 - t0);
    }
}
