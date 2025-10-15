#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <sys/fcntl.h>

#define TESTFILE "test.txt"
static void usage();

int main(int argc, char** argv) {
    bool use_stdio;
    if (argc < 2 || argc > 3) {
        usage();
    } else if (strcmp(argv[1], "-y") == 0) {
        use_stdio = false;
    } else if (strcmp(argv[1], "-s") == 0) {
        use_stdio = true;
    } else {
        usage();
    }

    // Overwrite `test.txt`
    FILE* f = fopen(TESTFILE, "w");
    fputs("CS 61 sucks\n", f);
    fclose(f);

    // Read first part of `test.txt`
    int fd = open(TESTFILE, O_RDONLY);
    assert(fd >= 0);

    char buf[1024];
    fprintf(stderr, "Reading 6 bytes from `test.txt`...\n");
    ssize_t nr;
    if (use_stdio) {
        f = fdopen(fd, "r");
        assert(f);
        nr = fread(buf, 1, 6, f);
    } else {
        nr = read(fd, buf, 6);
    }
    assert(nr == 6);

    // Give user a chance to play
    fprintf(stderr, "Now modify `test.txt` and then type `exit`!\n");
    int x = system("sh");
    (void) x;

    // Read rest of `test.txt`
    fprintf(stderr, "Reading more of `test.txt`...\n");
    ssize_t nr2;
    if (use_stdio) {
        nr2 = fread(&buf[nr], 1, sizeof(buf) - nr, f);
    } else {
        nr2 = read(fd, &buf[nr], sizeof(buf) - nr);
    }
    assert(nr2 >= 0 && size_t(nr + nr2) <= sizeof(buf));

    // Print the result
    fprintf(stderr, "And the answer is:\n");
    printf("%.*s\n", int(nr + nr2), buf);
}

static void usage() {
    fprintf(stderr, "Usage: ./coherence -y (syscalls) OR ./coherence -s (stdio)\n");
    exit(1);
}
