#include "u-lib.hh"

const char* messages[] = {
    "Wahoo!\n",
    "It's-a me, Mario!\n",
    "I'm-a piping.\n"
};

void process_main() {
    while (true) {
        // First, write a message.
        const char* message = messages[rand(0, arraysize(messages) - 1)];
        size_t pos = 0;
        size_t len = strlen(message);
        size_t nwrites = 0;
        while (pos < len) {
            ++nwrites;
            int r = sys_pipewritec(message[pos]);
            if (r == E_AGAIN) {
                continue;
            } else if (r <= 0) {
                panic("pipe write error");
            }
            ++pos;
        }

        // Print that message was written.
        console_printf(CS_WHITE "%zu write syscalls for %zuB: wrote %.*s",
                       nwrites, len, len, message);

        // Wait 1-3 seconds.
        unsigned long delay = rand(HZ, 3 * HZ - 1);
        unsigned long yields = delay * 5;
        unsigned long wait_until = ticks + delay;
        while (ticks < wait_until && yields > 0) {
            sys_yield();
            --yields;
        }
    }
}
