#include <cstdio>
#include <string>
#include "print_bytes.hh"


int main(int argc, char* argv[]) {
    // allocate space for integer versions of all the arguments
    int nargs = argc - 1;
    int* array = new int[nargs];

    // convert texts
    for (int i = 0; i != nargs; ++i) {
        array[i] = std::stoi(argv[i + 1]);
    }

    // add 'em up
    int sum = 0;
    for (int i = 0; i != nargs; ++i) {
        sum += array[i];
    }

    printf("sum of %d args = %d\n", nargs, sum);

    // free allocated space
    delete[] array;
}
