#include <cstdio>
#include <cassert>
#include <unistd.h>

int main() {
    const char* args[] = {
        "./myecho", // argv[0] is the string used to execute the program
        "Hello, world",
        nullptr
    };

    fprintf(stderr, "execmyecho[%d]: execing `./myecho`\n", getpid());

    int r = execvp(args[0], (char* const*) args);
    assert(r == 0);
}
