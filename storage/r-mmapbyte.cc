#include "iobench.hh"
#include <sys/mman.h>
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
    parse_arguments(argc, argv, &size, nullptr);

    char* buf = new char[1];

    start_tstamp = tstamp(); // include `mmap` in timing

    char* file_data = (char*) mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    if (file_data == (char*) MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    size_t n = 0;
    while (n < size) {
        memcpy(buf, &file_data[n], 1);
        n += 1;
        if (n % PRINT_FREQUENCY == 0) {
            report(n);
        }
    }

    munmap(file_data, size);
    close(fd);
    report(n, true);
    delete[] buf;
}
