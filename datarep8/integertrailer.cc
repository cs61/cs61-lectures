#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "hexdump.hh"

void add_trailer(char* ptr) {
    int trailer_value = 1039849126;
    // Note: The syntax `reinterpret_cast<int*>(&ptr[1999])` used here
    // means the same thing as `((int*) &ptr[1999])`.
    *reinterpret_cast<int*>(&ptr[1999]) = trailer_value;
}

int read_trailer(char* ptr) {
    return *reinterpret_cast<int*>(&ptr[1999]);
}

int main() {
    char* ptr = (char*) malloc(1999 + sizeof(int));
    memset(ptr, 0, 1999 + sizeof(int));

    add_trailer(ptr);

    printf("Trailer value is: %d\n", read_trailer(ptr));

    free(ptr);
}
