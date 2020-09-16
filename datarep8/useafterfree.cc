#include <cstdio>
#include <cstdlib>
#include <cstring>
#if __linux__
#include <malloc.h>
#endif
#include "hexdump.hh"

char* my_malloc(size_t sz) {
    return new char[sz];
}

void my_free(char* ptr) {
    delete[] ptr;
}


struct my_struct {
    char padding[24];
    const char* name;
    int number;
};

int main() {
    // allocate
    my_struct* ptr = (my_struct*) my_malloc(sizeof(my_struct));

    // check contents
    printf("name and number: %s:%d\n", ptr->name, ptr->number);

    // assign contents
    ptr->name = nullptr;
    ptr->number = 0;

    // check contents
    printf("name and number: %s:%d\n", ptr->name, ptr->number);

    // free
    my_free((char*) ptr);

    // check contents
    printf("name and number: %s:%d\n", ptr->name, ptr->number);
}
