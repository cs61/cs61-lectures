#include "arrayinfo.hh"
#include "hexdump.hh"

int main(int argc, char* argv[]) {
    array_info info = array_parse_arguments(argc, argv);
    double start = timestamp();

    // create sorted vector by insertion
    std::vector<int> seq;
    for (size_t i = 0; i != info.size; ++i) {
        int value = info.array[i];

        // find insertion position
        auto pos = std::find_if_not(seq.begin(), seq.end(),
                                    [=] (int x) { return x < value; });

        // insert value there
        seq.insert(pos, value);
    }

    printf("created list in %.06f sec\n", timestamp() - start);
    sequence_print(seq);

    // check that the collection is sorted
    assert(sequence_sorted(seq));
    // check that the sorted collection contains the same data
    // (this can be fooled, but it's better than nothing!)
    assert(sequence_sum(seq) == info.sum);

    delete[] info.array;
}
