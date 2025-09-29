//! -O3 -fno-tree-vectorize -fsanitize=undefined
#include <vector>

[[gnu::noinline]]
long sum(std::vector<long>& v) {
    long t = 0;
    for (auto i : v) {
        t += i;
    }
    return t;
}
