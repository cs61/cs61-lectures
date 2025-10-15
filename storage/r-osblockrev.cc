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
    off_t pos = size;
    size_t n = 0;
    while (pos > 0) {
        size_t nr = min((size_t) pos, block_size);
        pos -= nr;
        if (lseek(fd, pos, SEEK_SET) == (off_t) -1) {
            perror("lseek");
            exit(1);
        }
        ssize_t r = read(fd, buf, nr);
        if ((size_t) r != nr) {
            perror("read");
            exit(1);
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
