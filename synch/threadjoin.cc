#include <cstdio>
#include <thread>
#include <ctime>
#include <unistd.h>

void print1() {
    printf("Hello from thread 1\n");
}

void print_forever() {
    unsigned n = 0;
    while (true) {
        printf("Hello from thread 2, iteration %u\n", n);
        ++n;
        usleep(250'000);
    }
}

int main() {
    std::thread t1(print1);
    std::thread t2(print_forever);
    t1.join();
    // t2.detach();
    sleep(2);
}
