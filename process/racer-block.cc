#include "racer.hh"
bool verbose = false;
double work_time = 0.5;
double timeout = 0.75;
static volatile sig_atomic_t got_signal;

void signal_handler(int signal) {
    (void) signal;
    got_signal = 1;
}

int main(int argc, char** argv) {
    parse_arguments(argc, argv);

    // Demand that SIGCHLD interrupt system calls
    int r = set_signal_handler(SIGCHLD, signal_handler);
    assert(r >= 0);

    double start_time = timestamp();

    // Start child
    pid_t p1 = racer_fork(work_time);

    // Wait for the timeout or child exit.
    // `dsleep` will either succeed, returning 0 after `timeout` sec,
    // or be interrupted by SIGCHLD, returning -1. (Maybe.)
    r = dsleep(timeout);

    // Print results
    int status;
    pid_t wp = waitpid(p1, &status, WNOHANG);
    double elapsed = timestamp() - start_time;
    if (elapsed >= timeout) {
        printf("SLOW%s\n", racer_status(elapsed, wp, status).c_str());
    } else {
        printf("quick%s\n", racer_status(elapsed, wp, status).c_str());
    }
}
