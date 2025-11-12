#include "print_bytes.hh"
#include <cassert>
#include <cstdint>

static void fprint_bytes_ascii(FILE* f, const void* ptr, size_t size,
                               bool ascii);
static void fprint_ascii(FILE* f, const unsigned char* p, size_t pos);

void print_bytes(const void* ptr, size_t size) {
    fprint_bytes_ascii(stdout, ptr, size, false);
}

void print_bytes_ascii(const void* ptr, size_t size) {
    fprint_bytes_ascii(stdout, ptr, size, true);
}

void fprint_bytes(FILE* f, const void* ptr, size_t size) {
    fprint_bytes_ascii(f, ptr, size, false);
}

void fprint_bytes_ascii(FILE* f, const void* ptr, size_t size) {
    fprint_bytes_ascii(f, ptr, size, true);
}

void fprint_bytes_ascii(FILE* f, const void* ptr, size_t size, bool ascii) {
    const unsigned char* byteptr = reinterpret_cast<const unsigned char*>(ptr);
    for (size_t i = 0; i != size; ++i) {
        if (i % 16 == 0) {
            uintptr_t address = reinterpret_cast<uintptr_t>(ptr) + i;
            // print `address` with apostrophe separators
            for (int shift = sizeof(address) * 8 - 16; shift >= 0; shift -= 16) {
                uintptr_t chunk = address >> shift;
                if (chunk == 0 && shift > 16) {
                    continue;
                }
                const char* fmt;
                if (shift > 16 && chunk <= 0xffff) {
                    fmt = "%x'";
                } else if (shift > 0) {
                    fmt = "%04x'";
                } else {
                    fmt = "%04x";
                }
                fprintf(f, fmt, chunk & 0xffff);
            }
        }
        fprintf(f, "%s%02x", (i % 8 == 0 ? "  " : " "), byteptr[i]);
        if (i % 16 == 15 || i == size - 1) {
            if (ascii) {
                fprint_ascii(f, byteptr, i);
            } else {
                fputs("\n", f);
            }
        }
    }
}

static void fprint_ascii(FILE* f, const unsigned char* byteptr, size_t pos) {
    // Print an ASCII report that ends with byte p[pos].
    // The first byte printed is p[first], where first is the max multiple
    // of 16 having `first < pos`. The report starts at column 51.
    size_t first = pos - (pos % 16);  // first char to print
    int n = pos + 1 - first;          // # chars to print
    char buf[17];
    for (size_t i = first; i != first + n; ++i) {
        auto b = byteptr[i];
        buf[i - first] = (b >= 32 && b < 127 ? b : '.');
    }
    fprintf(f, "%*s|%.*s|\n", 51 - (3 * n + (n > 8)), "", n, buf);
}
