#include <cstdio>
#include "print_bytes.hh"

[[gnu::noinline]]
void hello() {
    printf("Hello!\n");
}

int main() {
    print_bytes(reinterpret_cast<const void*>(hello), 16);
    hello();
    print_bytes(reinterpret_cast<const void*>(hello), 16);
}
