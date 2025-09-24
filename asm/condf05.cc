//! -O3
#include <climits>
#include <limits>

bool check_overflow(int a, int b) {
    int sum = static_cast<int>(static_cast<unsigned>(a) + static_cast<unsigned>(b));
    if (a < 0) {
        return sum > b;
    } else {
        return sum < b;
    }
}

bool check_overflow(unsigned a, unsigned b) {
    return a > UINT_MAX - b;
}
