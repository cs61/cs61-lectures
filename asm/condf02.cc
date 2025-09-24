//! -O3

unsigned calc2(unsigned input, unsigned n) {
    unsigned sum = 0;
    for (unsigned i = 0; i < n; ++i) {
        sum += input;
        input = input * 1003 + n;
    }
    return sum;
}
