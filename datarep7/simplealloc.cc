#include <cstdio>

int main(int argc, char* argv[]) {
    (void) argv;

    char* ptr = new char[argc];
    printf("Allocated\n");
    delete[] ptr;
    printf("Deleted\n");
}
