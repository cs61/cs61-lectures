#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cerrno>
#include <cstdint>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern "C" {

int add(int a, int b) {
    // Open a file
    const char* file = "cs61hello.jpg";
    int fd = open(file, O_RDONLY);
    assert(fd >= 0);

    // Look up its size
    struct stat s;
    int r = fstat(fd, &s);
    assert(r >= 0 && S_ISREG(s.st_mode) && s.st_size > 0);

    // Load it into memory starting at address `data`
    void* data = mmap(nullptr, s.st_size, PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
    assert(data != MAP_FAILED);

    // Obtain address of add function in loaded file
    uintptr_t function_address = (uintptr_t) data + 0x9efc;
    int (*function_pointer)(int, int) = (int (*)(int, int)) function_address;

    // Call add function
    return function_pointer(a, b);
}

}
