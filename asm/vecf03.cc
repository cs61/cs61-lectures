//! -O3 -fno-tree-vectorize
#include <cstdio>

int dynamic_memory() {
    int* x = new int{4};
    printf("%p: %d\n", x, *x);
    delete x;
    return 0;
}
