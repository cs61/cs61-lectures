#include "arrayinfo.hh"
#include "hexdump.hh"

int main(int argc, char* argv[]) {
    // parse arguments to determine an array size and argument parse order
    array_info info = array_parse_arguments(argc, argv);

    // We expect the array to have the numbers 0 through `size - 1` in some
    // order. So their sum should equal `size * (size - 1) / 2`.
    long expected_sum = ((long) info.size * (long) (info.size - 1)) / 2;
    assert(expected_sum == info.sum);

    // initialize data array
    int* data = new int[info.size];
    array_initialize_up(data, info.size);

    printf("accessing data in %s order:\n", info.pattern);
    sequence_print(info.array, info.size);
    double start = timestamp();

    // sum elements of `data` array, accessed in the order defined by the
    // argument
    long sum = 0;
    for (size_t i = 0; i != info.size; ++i) {
        int data_index = info.array[i];
        assert(data_index >= 0 && data_index < (int) info.size);
        sum += data[data_index];
    }

    // check the checksum
    assert(sum == expected_sum);
    printf("OK in %.06f sec!\n", timestamp() - start);

    delete[] info.array;
    delete[] data;
}
