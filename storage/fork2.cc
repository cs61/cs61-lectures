#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

int main() {
    ssize_t nw = write(STDOUT_FILENO, "Hello!\n", 7);
    assert(nw == 7);

    pid_t p1 = fork();
    assert(p1 >= 0);

    if (p1 != 0) {
        char buf[1024];
        ssize_t n = snprintf(buf, sizeof(buf), "Child process pid was %d\n", p1);
        nw = write(STDOUT_FILENO, buf, n);
        assert(nw == n);
    }
    exit(0);
}
