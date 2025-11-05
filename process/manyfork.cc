#include <unistd.h>
#include "print_bytes.hh"

int main() {
    int nprocesses = 10000;
    double start_time = timestamp();
    for (int i = 0; i != nprocesses; ++i) {
        pid_t p1 = fork();
        assert(p1 >= 0);
        if (p1 == 0) {
            exit(0);
        }
    }
    double delta = timestamp() - start_time;
    printf("%d processes in %g sec (%g proc/sec)\n", nprocesses,
           delta, nprocesses / delta);
    sleep(10);
}
