#include "u-lib.hh"

void process_main() {
    unsigned n = 0;
    while (true) {
        ++n;
        if (n % 1024 == 0) {
            console_printf(CS_NORMAL "Hi, I'm Alice! #%u\n", n);
        }
        sys_yield();
    }
}
