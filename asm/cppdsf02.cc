#include <set>

[[gnu::noinline]]
long sum(std::set<long> v) {
    long t = 0;
    for (auto i : v) {
        t += i;
    }
    return t;
}
