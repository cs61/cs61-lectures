//! -O3 -fcf-protection=none
extern "C" {

int add(int a, int b) {
    while (a > 0) {
        a -= 4;
        b += 4;
    }
    while (a < 0) {
        a += 2;
        b -= 2;
    }
    if (a > 0) {
        ++b;
    }
    return b;
}

}
