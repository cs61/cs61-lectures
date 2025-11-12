#include <cstdio>
#include <thread>

void threadfunc(const char* message) {
    sched_yield();
    while (*message) {
        fputc(*message, stdout);
        ++message;
    }
}

int main() {
    std::thread t1(threadfunc, "Hello from thread 1\n");
    std::thread t2(threadfunc, "Hello from thread 2\n");
    t1.join();
    t2.join();
}
