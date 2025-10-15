#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

int main() {
    printf("Hello!\n");

    pid_t p1 = fork();
    assert(p1 >= 0);

    if (p1 != 0) {
        printf("Child process pid was %d\n", p1);
    }
    exit(0);
}
