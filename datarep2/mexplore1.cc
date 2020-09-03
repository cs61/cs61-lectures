#include "hexdump.hh"
#include <cstring>

int main() {
    char s1[] = "Hello, world";

    hexdump_object(s1);

    const char* s2 = "Hello, world";

    hexdump_object(s2);
    hexdump(s2, strlen(s2));
}
