#include "membench.hh"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <atomic>
#include <pthread.h>
#include <unistd.h>
#include <sys/resource.h>

static unsigned noperations;
static std::atomic<unsigned long> answer;

void* benchmark_thread(void* user_data) {
    answer += memnode_benchmark(noperations, (size_t) user_data);
    return nullptr;
}

int main(int argc, char** argv) {
    noperations = 10000000;
    int nthreads = 1;

    int opt;
    while ((opt = getopt(argc, argv, "n:j:m:")) != -1) {
        switch (opt) {
        case 'n': {
            char* end;
            noperations = strtoul(optarg, &end, 0);
            if (*end == 'm' || *end == 'M') {
                noperations *= 1000000;
            }
            break;
        }
        case 'j':
            nthreads = strtol(optarg, nullptr, 0);
            break;
        case 'm': {
            struct rlimit rlim;
            getrlimit(RLIMIT_AS, &rlim);
            char* end;
            rlim.rlim_cur = strtoul(optarg, &end, 0);
            if (*end == 'm' || *end == 'M') {
                rlim.rlim_cur *= 1000000;
            }
            if (rlim.rlim_cur <= 0) {
                rlim.rlim_cur = 10000000;
            }
            int r = setrlimit(RLIMIT_AS, &rlim);
            assert(r >= 0);
            break;
        }
        default:
            fprintf(stderr, "Usage: %s [-n NOPS] [-j NTHREADS] [-m MEMLIMIT]\n\
  Default NOPS %u, NTHREADS 1, no MEMLIMIT", argv[0], noperations);
            exit(1);
        }
    }

    if (nthreads <= 1) {
        // Run the benchmark.
        answer = memnode_benchmark(noperations, 1);
    } else {
        // Run `nthreads` threads, each running the benchmark.
        pthread_t threads[100];
        assert(nthreads <= 100);
        for (int i = 0; i != nthreads; ++i) {
            uintptr_t arg = 2 * i + 1;
            pthread_create(&threads[i], nullptr,
                           benchmark_thread, (void*) arg);
        }
        for (int i = 0; i != nthreads; ++i) {
            pthread_join(threads[i], nullptr);
        }
    }

    printf("nops %u, nthreads %d, answer %lu\n",
           noperations, nthreads, answer.load());
}
