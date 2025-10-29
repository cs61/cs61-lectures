#include <cstdio>
#include <cstring>
#include <cassert>
#include <csignal>
#include <cerrno>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t initial_pid = getpid();
    fprintf(stderr, "Hello from initial process, pid %d\n", initial_pid);
    int r;


    int pfd[2];
    r = pipe(pfd);
    // pfd[0] reads; pfd[1] writes
    assert(r >= 0);

    pid_t pa = fork();
    assert(pa >= 0);
    if (pa == 0) {
        dup2(pfd[1], 1);
        close(pfd[0]);
        close(pfd[1]);
        const char* args[] = {"echo", "rororrroororoorororooror", nullptr };
        r = execvp(args[0], (char* const*) args);
        assert(false);
    }

    close(pfd[1]);

    int pfd2[2];
    r = pipe(pfd2);
    assert(r >= 0);

    pid_t pb = fork();
    if (pb == 0) {
        dup2(pfd[0], 0);
        dup2(pfd2[1], 1);
        close(pfd[0]);
        close(pfd2[0]);
        close(pfd2[1]);
        const char* args[] = { "tr", "-d", "o", nullptr };
        r = execvp(args[0], (char* const*) args);
        assert(false);
    }

    close(pfd[0]);
    close(pfd2[1]);

    pid_t pc = fork();
    if (pc == 0) {
        dup2(pfd2[0], 0);
        close(pfd2[0]);
        const char* args[] = { "wc", "-c", nullptr };
        r = execvp(args[0], (char* const*) args);
        assert(false);
    }

    close(pfd2[0]);
    int status;
    pid_t w = waitpid(pc, &status, 0);
    assert(w == pc);
    _exit(0);
}
