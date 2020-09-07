#include "hexdump.hh"
#include <ctime>

int main() {
    char a[4];

    a[0] = 61;
    a[1] = 62;
    a[2] = 63;
    a[3] = 64;

    hexdump_object(a);
}
