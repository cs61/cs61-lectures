#include <cstdio>
#include <thread>

void threadfunc1() {
    printf("Hello from thread 1\n");
}

void threadfunc2() {
    printf("Hello from thread 2\n");
}

int main() {
    std::thread t1(threadfunc1);
    std::thread t2(threadfunc2);
    t1.join();
    t2.join();
}
