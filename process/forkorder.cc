#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cassert>
#include "print_bytes.hh"

int main() {
    setvbuf(stdout, nullptr, _IOLBF, 0);
    double start_time = timestamp();

    while (true) {
        double t = timestamp() - start_time;
        (void) t;

        pid_t p = fork();
        assert(p >= 0);
        if (p == 0) {
            //printf("%.09f: child[%d]\n", t, getpid());
            printf("child\n");
            exit(0);
        }

        //printf("%.09f: parent[%d]\n", t, p);
        printf("parent\n");
        int status;
        pid_t wp = waitpid(p, &status, 0);
        assert(wp == p);
    }
}
