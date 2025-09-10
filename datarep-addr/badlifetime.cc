#include <cstdio>
int* global_ptr;

int* lifetime_error_1() {
    int local = 5;
    return &local;
}

int* lifetime_error_2() {
    int* dynamic = new int{5};
    delete dynamic;
    return dynamic;
}

int* lifetime_error_3() {
    int local = 61;
    global_ptr = &local;
    return global_ptr;
}

int main() {
    int* uh_oh = lifetime_error_1();
    *uh_oh = 2;

    printf("Phew\n");
}
