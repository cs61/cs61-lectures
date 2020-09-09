#include "membench.hh"
#include <vector>
#include <string>

struct memnode {
    std::string name;
    unsigned number;
};

struct memnode_arena {
    memnode* allocate() {
        return new memnode;
    }
    void deallocate(memnode* m) {
        delete m;
    }
};


unsigned long memnode_benchmark(unsigned noperations, unsigned step) {
    assert(step % 2 == 1);  // `step` must be odd
    long counter = 0;
    memnode_arena arena;

    // Allocate 4096 memnodes.
    const unsigned nnodes = 4096;
    memnode* m[nnodes];
    for (unsigned i = 0; i != nnodes; ++i) {
        m[i] = arena.allocate();
        m[i]->name = "initial memnode";
        m[i]->number = counter;
        ++counter;
    }

    // Replace `noperations` memnodes.
    for (unsigned i = 0; i != noperations; ++i) {
        unsigned pos = (i * step) % nnodes;
        arena.deallocate(m[pos]);

        m[pos] = arena.allocate();
        m[pos]->name = "replaced memnode";
        m[pos]->number = counter;
        ++counter;
    }

    // Compute a statistic from the remaining memnodes and free them.
    unsigned long result = 0;
    for (unsigned i = 0; i != nnodes; ++i) {
        result += m[i]->number;
        arena.deallocate(m[i]);
    }

    return result;
}
