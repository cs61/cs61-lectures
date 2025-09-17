//! -O3 -fno-tree-vectorize
#include <vector>

int vecsum(std::vector<int>& v) {
    int sum = 0;
    for (auto& i : v) {
        sum += i;
    }
    return sum;
}
