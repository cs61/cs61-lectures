#include "hexdump.hh"

static void fhexdump_ascii(FILE* f, const unsigned char* p, size_t pos);

void hexdump(const void* ptr, size_t size) {
    fhexdump_at(stdout, (size_t) ptr, ptr, size);
}

void fhexdump(FILE* f, const void* ptr, size_t size) {
    fhexdump_at(f, (size_t) ptr, ptr, size);
}

void fhexdump_at(FILE* f, size_t first_offset, const void* ptr, size_t size) {
    const unsigned char* p = (const unsigned char*) ptr;
    for (size_t i = 0; i != size; ++i) {
        if (i % 16 == 0) {
            fprintf(f, "%08zx", first_offset + i);
        }
        fprintf(f, "%s%02x", (i % 8 == 0 ? "  " : " "), (unsigned) p[i]);
        if (i % 16 == 15 || i == size - 1) {
            fhexdump_ascii(f, p, i);
        }
    }
}

static void fhexdump_ascii(FILE* f, const unsigned char* p, size_t pos) {
    // Print an ASCII report that ends with byte p[pos].
    // The first byte printed is p[first], where first is the max multiple
    // of 16 having `first < pos`. The report starts at column 51.
    size_t first = pos - (pos % 16);  // first char to print
    int n = pos + 1 - first;          // # chars to print
    char buf[17];
    for (size_t i = first; i != first + n; ++i) {
        buf[i - first] = (p[i] >= 32 && p[i] < 127 ? p[i] : '.');
    }
    fprintf(f, "%*s|%.*s|\n", 51 - (3 * n + (n > 8)), "", n, buf);
}
