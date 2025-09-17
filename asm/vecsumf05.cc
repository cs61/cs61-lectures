//! -O3 -fno-tree-vectorize
#include <vector>

[[gnu::noinline]] int g(int);

int vecsum(std::vector<int>& v) {
    int sum = 0;
    for (auto& i : v) {
        sum += g(i);
    }
    return sum;
}
