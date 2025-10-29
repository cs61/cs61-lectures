#include "racer.hh"
bool verbose = false;
double work_time = 0.5;
double timeout = 0.75;

void signal_handler(int signal) {
    (void) signal;
}

int main(int argc, char** argv) {
    parse_arguments(argc, argv);

    // Demand that SIGALRM interrupt system calls
    int r = set_signal_handler(SIGALRM, signal_handler);
    assert(r >= 0);

    double start_time = timestamp();

    // Start child
    pid_t p1 = racer_fork(work_time);

    // Set an alarm for `timeout` sec, after which kernel delivers SIGALRM
    struct itimerval itimer;
    timerclear(&itimer.it_interval);
    itimer.it_value = make_timeval(timeout);
    r = setitimer(ITIMER_REAL, &itimer, nullptr);
    assert(r >= 0);

    // Wait for child to exit or alarm; alarm will interrupt `waitpid`
    int status;
    pid_t wp = waitpid(p1, &status, 0);
    double elapsed = timestamp() - start_time;
    if (elapsed >= timeout) {
        printf("SLOW%s\n", racer_status(elapsed, wp, status).c_str());
    } else {
        printf("quick%s\n", racer_status(elapsed, wp, status).c_str());
    }
}
