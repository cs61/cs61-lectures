#ifndef ARRAYINFO_HH
#define ARRAYINFO_HH
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <sys/time.h>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>

struct array_info {
    int* array;
    size_t size;
    const char* pattern;
    long sum;
};
array_info array_parse_arguments(int argc, char** argv);

void array_initialize_random(int* array, size_t size);
void array_initialize_up(int* array, size_t size);
void array_initialize_down(int* array, size_t size);
void array_initialize_magic(int* array, size_t size);


// sequence_sum(sequence)
//    Returns the sum of the elements in the argument sequence.
//    Versions are provided for simple arrays, C++ `std::list`,
//    and C++ `std::vector`.

inline long sequence_sum(const int* seq, size_t size) {
    long sum = 0;
    for (size_t i = 0; i != size; ++i) {
        sum += seq[i];
    }
    return sum;
}

inline long sequence_sum(const std::list<int>& seq) {
    long sum = 0;
    for (auto it = seq.begin(); it != seq.end(); ++it) {
        sum += *it;
    }
    return sum;
}

inline long sequence_sum(const std::vector<int>& seq) {
    long sum = 0;
    for (auto it = seq.begin(); it != seq.end(); ++it) {
        sum += *it;
    }
    return sum;
}


// sequence_sorted(seq)
//    Returns true iff the elements in the input sequence are stored
//    in non-decreasing order.
//    Versions are provided for simple arrays, C++ `std::list`,
//    and C++ `std::vector`.

inline bool sequence_sorted(const int* seq, size_t size) {
    for (size_t i = 1; i < size; ++i) {
        if (seq[i - 1] > seq[i]) {
            return false;
        }
    }
    return true;
}

inline bool sequence_sorted(const std::list<int>& seq) {
    if (!seq.empty()) {
        auto prev = seq.begin(), cur = prev;
        for (++cur; cur != seq.end(); ++prev, ++cur) {
            if (*prev > *cur) {
                return false;
            }
        }
    }
    return true;
}

inline bool sequence_sorted(const std::vector<int>& seq) {
    if (!seq.empty()) {
        auto prev = seq.begin(), cur = prev;
        for (++cur; cur != seq.end(); ++prev, ++cur) {
            if (*prev > *cur) {
                return false;
            }
        }
    }
    return true;
}


// sequence_print(seq)
//    Prints the first elements in the input sequence to standard output.
void sequence_print(const int* seq, size_t size);
void sequence_print(const std::list<int>& seq);
void sequence_print(const std::vector<int>& seq);


// sequence_append(a, b)
//    Returns a new sequence comprising the elements of `a` followed by
//    the elements of `b`.

inline std::list<int> sequence_append(const std::list<int>& a,
                                      const std::list<int>& b) {
    std::list<int> x;
    std::copy(a.begin(), a.end(), std::back_inserter(x));
    std::copy(b.begin(), b.end(), std::back_inserter(x));
    return x;
}

inline std::vector<int> sequence_append(const std::vector<int>& a,
                                        const std::vector<int>& b) {
    std::vector<int> x;
    std::copy(a.begin(), a.end(), std::back_inserter(x));
    std::copy(b.begin(), b.end(), std::back_inserter(x));
    return x;
}


// timestamp()
//    Return the current time in seconds.

inline double timestamp() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + (double) tv.tv_usec * 1e-6;
}

#endif
