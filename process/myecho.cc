#include <cstdio>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // behave like `/bin/echo`
    for (int i = 1; i != argc; ++i) {
        printf("%s%s", i == 1 ? "" : " ", argv[i]);
    }
    printf("\n");

    // additionally print pid and pointer information
    printf("[pid %d, argv {%p=\"%s\"", getpid(), argv[0], argv[0]);
    for (int i = 1; i != argc; ++i) {
        printf(", %p", argv[i]);
    }
    printf("}]\n");
}
