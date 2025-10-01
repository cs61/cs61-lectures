#include "u-lib.hh"

[[gnu::noinline]]
static int incr(int x) {
    return x + 1;
}

void process_main() {
    // print first 8 bytes of `incr` function
    //hexdump((void*) incr, 8, "incr");

    int x = sys_getpid();

    int x_incr = incr(x);
    assert(x_incr > x);

    char buf[512];
    size_t n = snprintf(buf, sizeof(buf), "assertion passed, so %d > %d\n", x_incr, x);
    ssize_t nw = sys_write(buf, n);
    assert((size_t) nw == n);

    while (true) {
        sys_yield();
    }
}
