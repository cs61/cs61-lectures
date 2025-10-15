#include "u-lib.hh"

void process_main() {
    char buf[200];
    size_t buflen = 0;
    size_t nreads = 0;

    while (true) {
        // read bytes from pipe
        ++nreads;
        int ch = sys_pipereadc();
        if (ch == E_AGAIN) {
            continue;
        } else if (ch < 0) {
            panic("pipe read error");
        }

        buf[buflen] = ch;
        ++buflen;

        // print any completed messages
        while (void* newline = memchr(buf, '\n', buflen)) {
            size_t linelen = reinterpret_cast<const char*>(newline) + 1 - buf;
            console_printf(CS_YELLOW "%zu read syscalls for %zuB: read %.*s",
                           nreads, linelen, linelen, buf);
            memmove(buf, buf + linelen, buflen - linelen);
            buflen -= linelen;
            nreads = 0;
        }
    }
}
