#include "u-lib.hh"

void process_main() {
    pid_t other_pid = sys_spawn("eve");

    unsigned n = 0;
    while (true) {
        ++n;
        if (n % 1024 == 0) {
            console_printf(CS_WHITE "Hi, I'm Sheng! I started PID %d. #%u\n",
                           other_pid, n);
        }
        sys_yield();
    }
}
