#include "hexdump.hh"

char ch1 = 'A';
const char ch2 = 'B';

void f() {
    char ch3 = 'C';
    char* ch4 = new char{'D'};

    hexdump(&ch1, sizeof(ch1));
    hexdump(&ch2, sizeof(ch2));
    hexdump(&ch3, sizeof(ch3));
    hexdump(ch4, sizeof(*ch4));
}

int main() {
    f();
}
