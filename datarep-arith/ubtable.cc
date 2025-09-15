#include <cassert>
#include <string>
#include <cstdlib>
#include <cstdio>

int awesome[4] = {0, 1, 2, 3};

bool is_awesome(int v) {
    for (int i = 0; i <= 4; ++i) {
        if (awesome[i] == v)
            return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
        exit(1);
    }

    int n = std::stoi(argv[1], nullptr, 0);
    printf("%d %s awesome\n", n, is_awesome(n) ? "is" : "is not");
}
