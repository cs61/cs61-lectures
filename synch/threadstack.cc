#include <cstdio>
#include <thread>
#include "print_bytes.hh"

void threadfunc(int id) {
    int local = id;
    print_object(local);
}

int main() {
    int local = 0;
    print_object(local);

    std::thread t1(threadfunc, 1);
    std::thread t2(threadfunc, 2);
    t1.join();
    t2.join();
}
