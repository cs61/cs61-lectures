#include "racer.hh"
bool verbose = false;
double work_time = 0.5;
double timeout = 0.75;

int main(int argc, char** argv) {
    parse_arguments(argc, argv);

    double start_time = timestamp();

    // Start child; exit after `exit_delay`
    pid_t p1 = fork();
    assert(p1 >= 0);
    if (p1 == 0) {
        if (work_time > 0) {
            dsleep(work_time);
        }
        _exit(0);
    }

    // Wait for timeout or child exit, whichever happens first
    int status;
    pid_t wp = 0;
    while (timestamp() - start_time < timeout && wp == 0) {
        wp = waitpid(p1, &status, WNOHANG);
        assert(wp == 0 || wp == p1);
    }

    // Print results
    double elapsed = timestamp() - start_time;
    if (elapsed >= timeout) {
        printf("SLOW%s\n", racer_status(elapsed, wp, status).c_str());
    } else {
        printf("quick%s\n", racer_status(elapsed, wp, status).c_str());
    }
}
