#include <cstdio>
#include <cassert>
#include <map>
#include "print_bytes.hh"

int main() {
    std::map<int, bool> ints;

    // read integers from stdin, storing them in sorted order
    int input;
    while (fscanf(stdin, "%d", &input) == 1) {
        ints.insert({input, true});
    }

    // print integers in sorted order
    for (auto& kvpair : ints) {
        // NB Elements of `std::map` are key-value pairs. We want the key.
        fprintf(stdout, "%d\n", kvpair.first);
    }
}
