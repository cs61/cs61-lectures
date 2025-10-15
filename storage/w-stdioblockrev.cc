#include "iobench.hh"
bool quiet = false;
double start_tstamp;

int main(int argc, char* argv[]) {
    FILE* f = stdout;
    if (isatty(fileno(f))) {
        f = fopen(DATAFILE, "w");
    }
    if (!f) {
        perror("fopen");
        exit(1);
    }

    size_t size = 51200000;
    size_t block_size = 512;
    parse_arguments(argc, argv, &size, &block_size);

    char* buf = new char[block_size];
    memset(buf, '6', block_size);

    start_tstamp = tstamp();
    off_t pos = size;
    size_t n = 0;
    while (pos > 0) {
        size_t nw = min((size_t) pos, block_size);
        pos -= nw;
        if (fseek(f, pos, SEEK_SET) == -1) {
            perror("fseek");
            exit(1);
        }
        size_t r = fwrite(buf, 1, nw, f);
        if (r != nw) {
            perror("fwrite");
            exit(1);
        }
        n += r;
        if (n % PRINT_FREQUENCY == 0) {
            report(n);
        }
    }

    fclose(f);
    report(n, true);
    delete[] buf;
}
