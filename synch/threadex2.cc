#include <cstdio>
#include <thread>

void threadfunc(int n) {
    printf("Hello from thread %d\n", n);
}

int main() {
    std::thread t1(threadfunc, 1);
    std::thread t2(threadfunc, 2);
    t1.join();
    t2.join();
}
