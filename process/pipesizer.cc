#include <unistd.h>
#include <cstdio>
#include <cassert>

int main() {
    int pipefd[2];
    int r = pipe(pipefd);
    assert(r >= 0);

    size_t x = 0;
    while (true) {
        ssize_t nw = write(pipefd[1], "!", 1);
        assert(nw == 1);
        ++x;
        printf("%zu\n", x);
    }
}
