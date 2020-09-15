#include <cstdlib>

void evil_function(char** pp) {
    (void) pp;

    // Your code here
}

int main() {
    char* ptrs[5];

    ptrs[0] = (char*) malloc(100);
    ptrs[1] = (char*) malloc(120);
    ptrs[2] = (char*) malloc(140);
    ptrs[3] = (char*) malloc(160);

    free(ptrs[1]);
    evil_function(&ptrs[2]);
}
