#include "u-lib.hh"

void process_main() {
    pid_t alice_pid = sys_spawn("alice");

    unsigned n = 0;
    while (true) {
        ++n;
        if (n % 1024 == 0) {
            console_printf(CS_YELLOW "Hi, I'm Sheng! I started %d. #%u\n",
                           alice_pid, n);
        }
        sys_yield();
    }
}
