#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <cstdlib>

int main() {
    // Create pipe
    int pfd[2];
    int r = pipe(pfd);
    assert(r == 0);

    // Write message to pipe
    const char* msg = "Hello!";
    ssize_t nw = write(pfd[1], msg, strlen(msg));
    assert(nw == (ssize_t) strlen(msg));

    // Read message from pipe
    char buf[BUFSIZ];
    ssize_t nr = read(pfd[0], buf, BUFSIZ);
    if (nr > 0) {
        printf("Received message “%.*s”\n", (int) nr, buf);
    } else {
        printf("Message failure\n");
    }
}
