#ifndef CS61_RACER_HH
#define CS61_RACER_HH
#include <unistd.h>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <csignal>
#include <ctime>
#include <string>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sched.h>
#include <errno.h>
#include "print_bytes.hh"

// dsleep(t)
//    Sleep for `t` seconds.

inline int dsleep(double t) {
    assert(t >= 0 && t < std::numeric_limits<useconds_t>::max() / 1'000'000);
    return usleep((useconds_t) (t * 1'000'000));
}


// racer_fork(work_time)
//    Create a child process that exits after `work_time` seconds. Returns the
//    child process’s pid.

inline pid_t racer_fork(double work_time) {
    pid_t p = fork();
    assert(p >= 0);
    if (p == 0) {
        if (work_time > 0) {
            dsleep(work_time);
        }
        _exit(0);
    }
    return p;
}


// racer_status(elapsed, waitresult, status)
//    Return human-readable information about the race.

std::string racer_status(double elapsed, pid_t waitresult, int status);


// make_timeval(t)
//    Return `t` as a `struct timeval`.

inline struct timeval make_timeval(double d) {
    double intpart;
    double fracpart = modf(d, &intpart);
    struct timeval tv = {
        (long) intpart, (int) (fracpart * 1'000'000)
    };
    return tv;
}


// set_signal_handler(signo, handler)
//    Install `handler` as the signal handler for `signo`.
//    The `handler` is automatically re-installed after signal delivery.
//    Has the same interface as `signal()` (`man 2 signal`), but is portable.

inline int set_signal_handler(int signo, void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler = handler;    // call `handler` on signal
    sigemptyset(&sa.sa_mask);   // don't block other signals in handler
    sa.sa_flags = 0;            // don't restart system calls
    return sigaction(signo, &sa, nullptr);
}


// parse_arguments(argc, argv)
//    Parse arguments for `-V`, `-e DELAY`, `-t DELAY`.

void parse_arguments(int argc, char* argv[]);

#endif
