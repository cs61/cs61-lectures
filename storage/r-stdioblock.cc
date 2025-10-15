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
    size_t n = 0;
    while (n < size) {
        size_t r = fread(buf, 1, block_size, f);
        if (r == 0 && ferror(f)) {
            perror("fread");
            exit(1);
        } else if (r != block_size) {
            break;
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
