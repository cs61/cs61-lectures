#ifndef CS61_HEXDUMP_HH
#define CS61_HEXDUMP_HH
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <cmath>

// hexdump(ptr, size)
//    Print a hexdump of the `size` bytes of data starting at `ptr`
//    to the standard output. The hexdump format is:
//    XXXXXXXX  BB BB BB BB BB BB BB BB  BB BB BB BB BB BB BB BB  |CCCCCCCCCCCCCCCC|
//    where XXXXXXXX is the address of the first byte in the line,
//    the BBs are hexadecimal values of those bytes, and the Cs show
//    the printable ASCII characters corresponding to those bytes
//    (`.` is displayed for non-printable characters).
void hexdump(const void* ptr, size_t size, const char* objname = nullptr);

// hexdump_object(object)
//    Like hexdump(&object, sizeof(object)).
#define hexdump_object(object) hexdump(&(object), sizeof((object)))

// hexdump_named_object(object)
//    Like hexdump(&object, sizeof(object)), but also print a header with
//    the name of `object`.
#define hexdump_named_object(object) hexdump(&(object), sizeof((object)), #object)


// fhexdump(f, ptr, size)
//    Like `hexdump(ptr, size)`, but print to file `f` rather than standard
//    output.
void fhexdump(FILE* f, const void* ptr, size_t size, const char* objname = nullptr);

// fhexdump_object(f, object)
//    Like `fhexdump(f, &object, sizeof(object))`.
#define fhexdump_object(f, object) fhexdump((f), &(object), sizeof((object)))

// fhexdump_named_object(f, object)
//    Like `fhexdump(f, &object, sizeof(object))`.
#define fhexdump_named_object(f, object) fhexdump((f), &(object), sizeof((object)), #object)


// fhexdump_at(f, first_offset, ptr, size)
//    Like fhexdump, but start with offset `first_offset` instead of the
//    address of `ptr`.
void fhexdump_at(FILE* f, size_t first_offset, const void* ptr, size_t size,
                 const char* objname = nullptr);


// cputime()
//    Return the amount of CPU time this process has taken so far.
inline double cputime() {
    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ts.tv_sec + (double) ts.tv_nsec * 1e-9;
}

// timestamp()
//    Return the current monotonic timestamp (wall clock time).
inline double timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + (double) ts.tv_nsec * 1e-9;
}

// elapsed_time()
//    Return the elapsed time since the first call.

inline double elapsed_time() {
    static double start_time = timestamp();
    return timestamp() - start_time;
}

// dsleep(d)
//    Sleep for at least `d` seconds.
inline int dsleep(double d) {
    double intpart;
    double fracpart = modf(d, &intpart);
    struct timespec ts = {
        (long) intpart, (long) (fracpart * 1'000'000'000)
    };
    return nanosleep(&ts, nullptr);
}

#endif
