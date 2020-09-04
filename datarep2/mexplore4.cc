#include "hexdump.hh"

void f() {
    int i1 = 0;
    int i2 = 1;
    int i3 = 2;
    char c1 = 3;
    char c2 = 4;
    char c3 = 5;

    hexdump(&i1, sizeof(i1));
    hexdump(&i2, sizeof(i2));
    hexdump(&i3, sizeof(i3));
    hexdump(&c1, sizeof(c1));
    hexdump(&c2, sizeof(c2));
    hexdump(&c3, sizeof(c3));
}

int main() {
    f();
}
