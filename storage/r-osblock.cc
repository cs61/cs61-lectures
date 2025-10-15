#include "iobench.hh"
bool quiet = false;
double start_tstamp;

int main(int argc, char* argv[]) {
    int fd = STDIN_FILENO;
    if (isatty(fd)) {
        fd = open(DATAFILE, O_RDONLY);
    }
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    size_t size = filesize(fd);
    size_t block_size = 512;
    parse_arguments(argc, argv, &size, &block_size);

    char* buf = new char[block_size];

    start_tstamp = tstamp();
    size_t n = 0;
    while (n < size) {
        ssize_t r = read(fd, buf, block_size);
        if (r == -1) {
            perror("read");
            exit(1);
        } else if (r != (ssize_t) block_size) {
            break;
        }
        n += r;
        if (n % PRINT_FREQUENCY == 0) {
            report(n);
        }
    }

    close(fd);
    report(n, true);
    delete[] buf;
}
