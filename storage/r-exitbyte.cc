#include "iobench.hh"
#include <sys/wait.h>
bool quiet = false;
double start_tstamp;

// Read one byte from `fd` by forking a child process, reading the byte in the
// child process, and communicating the byte back to the parent via exit
// status.
int exit_getc(int fd) {
    pid_t p = fork();
    if (p == 0) {
        unsigned char ch;
        ssize_t nr = read(fd, &ch, 1);
        if (nr == 1) {
            exit(ch);
        } else {
            // Terminate abnormally
            abort();
        }
    } else if (p < 0) {
        return -1;
    }

    int status;
    pid_t w = waitpid(p, &status, 0);
    if (w == p && WIFEXITED(status)) {
        unsigned char ch = WEXITSTATUS(status);
        return ch;
    } else {
        return -1;
    }
}


int main(int argc, char* argv[]) {
    FILE* f = stdin;
    if (isatty(fileno(f))) {
        f = fopen(DATAFILE, "r");
    }
    if (!f) {
        perror("fopen");
        exit(1);
    }

    size_t size = filesize(fileno(f));
    parse_arguments(argc, argv, &size, nullptr);

    start_tstamp = tstamp();

    size_t n = 0;
    while (n < size) {
        int ch = exit_getc(fileno(f));
        if (ch == EOF) {
            break;
        }
        n += 1;
        if (n % PRINT_FREQUENCY == 0) {
            report(n);
        }
    }

    fclose(f);
    report(n, true);
}
