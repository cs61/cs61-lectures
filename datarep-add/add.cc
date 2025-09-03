#include <cstdio>
#include <string>
#include "print_bytes.hh"
extern "C" {
int add(int a, int b);
}


int main(int argc, char* argv[]) {
    // we must have exactly 3 arguments (including the program name)
    assert(argc == 3);

    // convert texts to integers
    int a = std::stoi(argv[1]);
    int b = std::stoi(argv[2]);

    // print their sum
    printf("%d + %d = %d\n", a, b, add(a, b));
}
