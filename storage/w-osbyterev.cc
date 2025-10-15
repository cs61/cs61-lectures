#include "iobench.hh"
bool quiet = false;
double start_tstamp;

int main(int argc, char* argv[]) {
    int fd = STDOUT_FILENO;
    if (isatty(fd)) {
        fd = open(DATAFILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    size_t size = filesize(fd);
    parse_arguments(argc, argv, &size, nullptr);

    const char* buf = "6";

    start_tstamp = tstamp();
    off_t pos = size;
    size_t n = 0;
    while (pos > 0) {
        pos -= 1;
        if (lseek(fd, pos, SEEK_SET) == (off_t) -1) {
            perror("lseek");
            exit(1);
        }
        ssize_t r = write(fd, buf, 1);
        if (r != 1) {
            perror("write");
            exit(1);
        }
        n += r;
        if (n % PRINT_FREQUENCY == 0) {
            report(n);
        }
    }

    close(fd);
    report(n, true);
}
