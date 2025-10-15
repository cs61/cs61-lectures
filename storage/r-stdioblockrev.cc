#include "iobench.hh"
bool quiet = false;
double start_tstamp;

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
    size_t block_size = 512;
    parse_arguments(argc, argv, &size, &block_size);

    char* buf = new char[block_size];

    start_tstamp = tstamp();
    off_t pos = size;
    size_t n = 0;
    while (pos > 0) {
        size_t nr = min((size_t) pos, block_size);
        pos -= nr;
        if (fseek(f, pos, SEEK_SET) == -1) {
            perror("fseek");
            exit(1);
        }
        size_t r = fread(buf, 1, nr, f);
        if (r != nr) {
            perror("fread");
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
