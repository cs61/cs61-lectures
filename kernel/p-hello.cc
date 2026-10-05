#include "u-lib.hh"

void process_main() {
    sys_write("Hello, 61 world\n");
    while (true) {
        sys_yield();
    }
}
