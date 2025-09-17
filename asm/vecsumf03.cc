//! -O0 -fno-tree-vectorize
#include <cstddef>

int vecsum(int* data, size_t n) {
    int sum = 0;
    for (size_t i = 0; i != n; ++i) {
        sum += data[i];
    }
    return sum;
}
