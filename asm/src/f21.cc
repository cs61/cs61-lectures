//! -O2 -Wno-infinite-recursion

int f(int a) {
    return f(a + 1);
}
