#include "hexdump.hh"

struct example {
    const char* name1;
    char bytes[90];
    int ints[12];
    long longs[4];
    short shorts[1];
    char* bptrs[4];
    int* iptrs[4];
};

int main() {
    example e;

    e.name1 = "Name goes here";
    for (size_t i = 0; i != 90; ++i) {
        e.bytes[i] = i;
    }
    for (size_t i = 0; i != 12; ++i) {
        e.ints[i] = i;
    }
    for (size_t i = 0; i != 4; ++i) {
        e.longs[i] = i;
    }
    for (size_t i = 0; i != 1; ++i) {
        e.shorts[i] = i;
    }
    e.bptrs[0] = &e.bytes[10];
    e.bptrs[1] = &e.bytes[20];
    e.bptrs[2] = &e.bytes[30];
    e.bptrs[3] = &e.bytes[40];
    e.iptrs[0] = &e.ints[1];
    e.iptrs[1] = &e.ints[2];
    e.iptrs[2] = &e.ints[3];
    e.iptrs[3] = &e.ints[5];

    hexdump_object(e);
}
