#include "hexdump.hh"

int main() {
    int i = 61;

    // Print the bytes that make up the object `i`.
    hexdump_object(i);

    // This line does the same thing.
    hexdump(&i, sizeof(i));
}
