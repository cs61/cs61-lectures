//! -O2

unsigned sum2(const unsigned* a, unsigned n) {
    unsigned sum = 0;
    for (unsigned i = 0; i < n; ++i) {
        sum += a[i];
    }
    return sum;
}
