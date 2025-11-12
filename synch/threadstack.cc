#include <cstdio>
#include <cstring>
#include <thread>
#include <ctime>
#include <unistd.h>
#include "print_bytes.hh"

void print_string(const char* who, const char* str) {
    char buf[BUFSIZ];
    snprintf(buf, sizeof(buf), "%p in %s: “%s”\n", str, who, str);
    fputs(buf, stdout);
}

void threadfunc(int id,
                [[maybe_unused]] const char* readonly_str,
                [[maybe_unused]] char* caller_buffer,
                [[maybe_unused]] char* heap_buffer) {
    char my_name[100];
    snprintf(my_name, sizeof(my_name), "T%d", id);

    char my_buffer[100];
    snprintf(my_buffer, sizeof(my_buffer),
             "thread %d's buffer", id);

    print_string(my_name, my_buffer);

    //print_string(my_name, readonly_str);
    //print_string(my_name, caller_buffer);
    //print_string(my_name, heap_buffer);
}

int main() {
    char main_local_str[100];
    strcpy(main_local_str, "main's buffer");

    char* heap_str = new char[100];
    strcpy(heap_str, "heap buffer");

    std::thread t1(threadfunc, 1, "readonly", main_local_str, heap_str);
    std::thread t2(threadfunc, 2, "readonly", main_local_str, heap_str);
    t1.join();
    t2.join();

    print_string("MAIN", main_local_str);
}
