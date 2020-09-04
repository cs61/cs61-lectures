#include "hexdump.hh"

char ch1 = 'A';
const char ch2 = 'B';

void f() {
    char* ch1 = new char{'6'};
    char* ch2 = malloc(1);
    *ch2 = '1';

    hexdump(ch1, 1);
    hexdump(ch2, 1);
}

int main() {
    f();
}
