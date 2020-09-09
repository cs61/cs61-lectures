#include "arrayinfo.hh"
#include <cerrno>
#include <sys/resource.h>
void limit_stack_size(size_t size);


void array_initialize_up(int* array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        array[i] = i;
    }
}

void array_initialize_down(int* array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        array[i] = size - i - 1;
    }
}

void array_initialize_random(int* array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        array[i] = i;
    }
    for (size_t i = 0; i < size; ++i) {
        std::swap(array[i], array[random() % size]);
    }
}

void array_initialize_magic(int* array, size_t size) {
    array[0] = 0;
    for (size_t i = 1; i <= size; ++i) {
        size_t middle = i / 2;
        array[i - 1] = array[middle];
        array[middle] = i - 1;
    }
}


array_info array_parse_arguments(int argc, char** argv) {
    limit_stack_size(1048576);   // 1MB of stack is enough for anyone!

    array_info info;
    info.size = 6;

    // parse command line arguments
    int initialize_type = 'u';
    for (int i = 1; i < argc; ++i) {
        unsigned long size;
        char* ends;
        if (strcmp(argv[i], "-r") == 0
            || strcmp(argv[i], "-u") == 0
            || strcmp(argv[i], "-d") == 0
            || strcmp(argv[i], "-m") == 0) {
            initialize_type = argv[i][1];
        } else if (isdigit((unsigned char) argv[i][0])
                   && (size = strtoul(argv[i], &ends, 0)) > 0
                   && *ends == 0) {
            info.size = size;
        } else {
            fprintf(stderr, "Usage: %s [-r|-u|-d|-m] [SIZE]\n", argv[0]);
            exit(1);
        }
    }

    // initialize based on command line argument
    info.array = new int[info.size];
    if (initialize_type == 'u') {
        array_initialize_up(info.array, info.size);
        info.pattern = "sequential";
    } else if (initialize_type == 'd') {
        array_initialize_down(info.array, info.size);
        info.pattern = "reverse sequential";
    } else if (initialize_type == 'r') {
        array_initialize_random(info.array, info.size);
        info.pattern = "random";
    } else if (initialize_type == 'm') {
        array_initialize_magic(info.array, info.size);
        info.pattern = "magic";
    }

    info.sum = sequence_sum(info.array, info.size);
    return info;
}


void sequence_print(const int* array, size_t size) {
    printf("[");
    for (size_t i = 0; i < size && i < 20; ++i) {
        printf(i ? ", %d" : "%d", array[i]);
    }
    if (size > 20) {
        printf(", ...");
    }
    printf("]\n");
}

void sequence_print(const std::list<int>& seq) {
    printf("[");
    auto it = seq.begin();
    for (int i = 0; it != seq.end() && i < 20; ++i, ++it) {
        printf(i ? ", %d" : "%d", *it);
    }
    if (it != seq.end()) {
        printf(", ...");
    }
    printf("]\n");
}

void sequence_print(const std::vector<int>& seq) {
    printf("[");
    auto it = seq.begin();
    for (int i = 0; it != seq.end() && i < 20; ++i, ++it) {
        printf(i ? ", %d" : "%d", *it);
    }
    if (it != seq.end()) {
        printf(", ...");
    }
    printf("]\n");
}


void check_system_call(int r, const char* syscall) {
    if (r == -1) {
        fprintf(stderr, "%s: %s\n", syscall, strerror(errno));
        exit(1);
    }
}

void limit_stack_size(size_t size) {
    struct rlimit rlim;
    int r = getrlimit(RLIMIT_STACK, &rlim);
    check_system_call(r, "getrlimit");
    if (rlim.rlim_cur > size) {
        rlim.rlim_cur = size;
        r = setrlimit(RLIMIT_STACK, &rlim);
        check_system_call(r, "setrlimit");
    }
}
