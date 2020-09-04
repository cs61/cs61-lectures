#include "hexdump.hh"

void f(int i) {
    hexdump(&i, sizeof(i));

    if (i > 0) {
        f(i - 1);
    }

    hexdump(&i, sizeof(i));
}

int main() {
    f(5);
}
