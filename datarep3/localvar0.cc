#include "hexdump.hh"
#include <ctime>

struct example {
    int x;
    char y;
    int z;
    char w;
};

void f() {
    int x = 61;
    char y = 62;
    int z = 63;
    char w = 64;

    hexdump_object(x);
    hexdump_object(y);
    hexdump_object(z);
    hexdump_object(w);
};

int main() {
    f();
}
