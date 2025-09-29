#include <list>

[[gnu::noinline]]
long sum(std::list<long> v) {
    long t = 0;
    for (auto i : v) {
        t += i;
    }
    return t;
}
