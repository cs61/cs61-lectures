#include "hexdump.hh"
#include <cassert>

static void hexdump_ascii(char* s, char* ends, const unsigned char* p, size_t pos);

void hexdump(const void* ptr, size_t size, const char* objname) {
    fhexdump_at(stdout, (size_t) ptr, ptr, size, objname);
}

void fhexdump(FILE* f, const void* ptr, size_t size, const char* objname) {
    fhexdump_at(f, (size_t) ptr, ptr, size, objname);
}

void fhexdump_at(FILE* f, size_t first_offset, const void* ptr, size_t size,
                 const char* objname) {
    const char* addrfmt = objname ? "  %08zx" : "%08zx";
    const unsigned char* p = (const unsigned char*) ptr;
    if (objname != nullptr) {
        fprintf(f, "%s:\n", objname);
    }
    char line[120];
    char* pos = line;
    char* endline = line + sizeof(line);
    for (size_t i = 0; i != size; ++i) {
        if (i % 16 == 0) {
            pos += snprintf(pos, endline - pos,
                            addrfmt, first_offset + i);
        }
        assert(pos + 4 < endline);
        *pos++ = ' ';
        if (i % 8 == 0) {
            *pos++ = ' ';
        }
        *pos++ = "0123456789abcdef"[((unsigned char) p[i] >> 4) & 0xF];
        *pos++ = "0123456789abcdef"[(unsigned char) p[i] & 0xF];
        if (i % 16 == 15 || i == size - 1) {
            hexdump_ascii(pos, endline, p, i);
            fputs(line, f);
            pos = line;
        }
    }
}

static void hexdump_ascii(char* s, char* ends,
                          const unsigned char* p, size_t pos) {
    // Generate an ASCII report that ends with byte p[pos].
    // The first byte printed is p[first], where first is the max multiple
    // of 16 having `first < pos`. The report starts at column 51.
    size_t first = pos - (pos % 16);  // first char to print
    int n = pos + 1 - first;          // # chars to print

    size_t nsp = 51 - (3 * n + (n > 8));
    while (nsp > 0 && s != ends) {
        *s++ = ' ';
        --nsp;
    }

    if (s != ends) {
        *s++ = '|';
    }
    for (size_t i = first; i != first + n && s != ends; ++i) {
        *s++ = p[i] >= 32 && p[i] < 127 ? p[i] : '.';
    }
    if (s != ends) {
        *s++ = '|';
    }
    if (s != ends) {
        *s++ = '\n';
    }
    if (s != ends) {
        *s = '\0';
    } else {
        s[-1] = '\0';
    }
}
