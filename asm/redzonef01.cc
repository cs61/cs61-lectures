//! -O1 -Wno-unused -fno-stack-protector

[[gnu::noinline]] int g(int x);

int f(unsigned a) {
    int local[4] = { 61, 42981762, 391063, 2164 };
    return local[a & 3];
}
