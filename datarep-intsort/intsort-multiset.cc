#include <cstdio>
#include <cassert>
#include <set>
#include "print_bytes.hh"

int main() {
    std::multiset<int> ints;

    // read integers from stdin, storing them in sorted order
    int input;
    while (fscanf(stdin, "%d", &input) == 1) {
        ints.insert(input);
    }

    // print integers in sorted order
    for (auto& i : ints) {
        fprintf(stdout, "%d\n", i);
    }
}
