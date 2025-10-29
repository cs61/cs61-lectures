#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cassert>
#include "print_bytes.hh"

int main() {
    fprintf(stderr, "Hello from parent pid %d\n", getpid());

    // Start a child
    pid_t p1 = fork();
    assert(p1 >= 0);
    if (p1 == 0) {
        usleep(500000);
        fprintf(stderr, "Goodbye from child pid %d\n", getpid());
        exit(0);
    }
    double start_time = timestamp();

    // Wait for the child and print its status
    int status;
    pid_t exited_pid = waitpid(p1, &status, 0);
    assert(exited_pid == p1);

    if (WIFEXITED(status)) {
        fprintf(stderr, "Child exited with status %d after %g sec\n",
                WEXITSTATUS(status), timestamp() - start_time);
    } else {
        fprintf(stderr, "Child exited abnormally [%x]\n", status);
    }
}
