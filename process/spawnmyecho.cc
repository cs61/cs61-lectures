#include "helpers.hh"
#include <spawn.h>

int main() {
    const char* args[] = {
        "./myecho", // argv[0] is the string used to execute the program
        "Hello, world",
        nullptr
    };

    fprintf(stderr, "spawnmyecho[%d]: running\n", getpid());

    pid_t p;
    int r = posix_spawn(&p, "./myecho", nullptr, nullptr,
                        (char**) args, nullptr);

    assert(r == 0);
    fprintf(stderr, "spawnmyecho[%d]: child pid %d with status %d\n",
            getpid(), p, r);
}
