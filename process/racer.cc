#include "racer.hh"
#include <format>

// racer_status(elapsed, waitresult, status)
//    Return human-readable information about the race.

std::string racer_status(double elapsed, pid_t waitresult, int status) {
    extern bool verbose;
    if (!verbose) {
        return "";
    } else if (waitresult == -1) {
        return std::format(" {:.06f} (parent interrupted by {})", elapsed, strerror(errno));
    } else if (waitresult == 0) {
        return std::format(" {:.06f} (child has not exited)", elapsed);
    } else if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            return std::format(" {:.06f}", elapsed);
        }
        return std::format(" {:.06f} (child exited with status {})", elapsed, WEXITSTATUS(status));
    } else {
        return std::format(" {:.06f} (child terminated abnormally)", elapsed);
    }
}


// parse_arguments(argc, argv)
//    Parse arguments for `-V`, `-e DELAY`, `-t DELAY`.

void parse_arguments(int argc, char* argv[]) {
    extern bool verbose;
    extern double work_time, timeout;
    int ch;
    while ((ch = getopt(argc, argv, "Vw:t:h")) != -1) {
        double* ptr = nullptr;
        if (ch == 'w') {
            ptr = &work_time;
        } else if (ch == 't') {
            ptr = &timeout;
        } else if (ch == 'V') {
            verbose = true;
        } else if (ch == 'h') {
            fprintf(stderr, "Usage: %s [-w WORKTIME] [-t TIMEOUT] [-V]\n", argv[0]);
            fprintf(stderr, "Arguments are:\n\
  -w WORKTIME      Set work time in child\n\
  -t TIMEOUT       Set timeout in parent\n\
  -V               Be verbose (print elapsed time and child information)\n");
            exit(0);
        }
        if (ptr) {
            char* endptr;
            double val = strtod(optarg, &endptr);
            if (*endptr == '\0') {
                *ptr = val;
            }
        }
    }
}
