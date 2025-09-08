#include <cstdio>
#include <cassert>
#include <list>
#include <vector>
#include "print_bytes.hh"

int main() {
    std::vector<int> ints;

    // read integers from stdin, storing them in sorted order
    int input;
    while (fscanf(stdin, "%d", &input) == 1) {
        auto it = ints.begin();
        while (it != ints.end() && *it < input) {
            ++it;
        }
        ints.insert(it, input);
    }

    // print integers in sorted order
    for (auto& value : ints) {
        fprintf(stdout, "%d\n", value);
    }
}
