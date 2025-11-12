#include <cstdio>
#include <cassert>
#include <thread>

void setval(int& v) {
    printf("Setting %p to 1\n", &v);
    v = 1;
}

int main() {
    int i1 = 0, i2 = 0;
    std::thread t1(setval, std::ref(i1));
    std::thread t2(setval, std::ref(i2));
    t1.join();
    t2.join();
    assert(i1 == 1 && i2 == 1);
}
