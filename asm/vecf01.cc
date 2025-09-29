//! -O3 -fno-tree-vectorize
#include <vector>

[[gnu::noinline]]
long sum(std::vector<long> v) {
    long t = 0;
    for (auto i : v) {
        t += i;
    }
    return t;
}
