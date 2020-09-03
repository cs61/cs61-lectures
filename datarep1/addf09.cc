#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cerrno>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cinttypes>
#include <unistd.h>

// filesize(f)
//    Return the size of `f` in bytes. Returns -1 if `f` does not have a
//    well-defined size (for instance, if it is a pipe).
off_t filesize(int fd) {
    struct stat s;
    int r = fstat(fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode)) {
        return s.st_size;
    } else {
        return -1;
    }
}

extern "C" {

int add(int a, int b) {
    const char* file = "cs61hello-2020.jpg";
    size_t offset = 0x24A0;

    // Load the file into memory
    int fd = open(file, O_RDONLY);
    assert(fd >= 0);

    off_t size = filesize(fd);
    assert(fd >= 0);

    void* data = mmap(NULL, size, PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
    assert(data != MAP_FAILED);

    // Obtain address of "function" in loaded file
    uintptr_t data_address = (uintptr_t) data + offset;
    int (*add_function)(int, int) = (int (*)(int, int)) (data_address);

    return add_function(a, b);
}

}
