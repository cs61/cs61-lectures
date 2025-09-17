//! -O2

unsigned long f(unsigned long* a, unsigned long n) {
    unsigned long sum = 0;
    for (unsigned long i = 0; i != n; ++i) {
        sum += a[i];
    }
    return sum;
}
