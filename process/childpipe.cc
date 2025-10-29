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

    pid_t p1 = fork();
    assert(p1 >= 0);

    if (p1 == 0) {
        // Child: Write message to pipe, then exit
        const char* msg = "Hello, mama!";
        ssize_t nw = write(pfd[1], msg, strlen(msg));
        assert(nw == (ssize_t) strlen(msg));
        exit(0);
    }

    // Parent: Read all messages from child; exit when child
    // is done
    close(pfd[1]); // Parent does not write!!!!
    while (true) {
        char buf[BUFSIZ];
        ssize_t nr = read(pfd[0], buf, sizeof(buf));
        if (nr > 0) {
            printf("Received message “%.*s”\n", (int) nr, buf);
        } else if (nr == 0
                   || (nr == -1 && errno != EINTR && errno != EAGAIN)) {
            break;
        }
    }
    printf("No more messages :(\n");
    close(pfd[0]);
}
