//! -O1 -fno-tree-vectorize -fno-stack-protector -fno-exceptions
#include <vector>

[[gnu::noinline]]
long sum(std::vector<long> v) {
    long t = 0;
    for (auto i : v) {
        t += i;
    }
    return t;
}

long sum_n_longs(size_t n) {
    std::vector<long> v;
    while (v.size() < n) {
        v.push_back(v.size());
    }
    return sum(v);
}
