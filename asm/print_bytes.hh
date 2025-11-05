#ifndef CS61_PRINT_BYTES_HH
#define CS61_PRINT_BYTES_HH
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <ctime>

// print_bytes(ptr, size)
//    Print a hexdump of the `size` bytes of data starting at `ptr`
//    to the standard output. The hexdump format is:
//    XXXXXXXX  BB BB BB BB BB BB BB BB  BB BB BB BB BB BB BB BB
//    where XXXXXXXX is the address of the first byte in the line,
//    the BBs are hexadecimal values of those bytes.
void print_bytes(const void* ptr, size_t size);

// print_object(object)
//    Like print_bytes(&object, sizeof(object)).
#define print_object(object) \
    print_bytes(&(object), sizeof((object)))

// print_bytes_ascii(ptr, size)
//    Print a hexdump of the `size` bytes of data starting at `ptr`,
//    including ASCII characters. The format is:
//    XXXXXXXX  BB BB BB BB BB BB BB BB  BB BB BB BB BB BB BB BB  |CCCCCCCCCCCCCCCC|
//    where XXXXXXXX and BB are as in print_bytes, and the Cs show any
//    printable ASCII characters corresponding to those bytes.
void print_bytes_ascii(const void* ptr, size_t size);

// print_object_ascii(object)
//    Like print_bytes_ascii(&object, sizeof(object)).
#define print_object_ascii(object) \
    print_bytes_ascii(&(object), sizeof((object)))


// fprint_bytes(f, ptr, size)
//    Like `print_bytes(ptr, size)`, but print to file `f` rather than standard
//    output.
void fprint_bytes(FILE* f, const void* ptr, size_t size);

#define fprint_object(f, object) \
    fprint_bytes((f), &(object), sizeof((object)))

void fprint_bytes_ascii(FILE* f, const void* ptr, size_t size);

#define fprint_object_ascii(f, object) \
    fprint_bytes_ascii((f), &(object), sizeof((object)))


// timestamp()
//    Return the current monotonic time.
inline double timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + (double) ts.tv_nsec * 1e-9;
}

// cputime()
//    Return the amount of CPU time this process has taken so far.
inline double cputime() {
    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ts.tv_sec + (double) ts.tv_nsec * 1e-9;
}

#endif
