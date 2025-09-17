//! -O1 -fcf-protection=none
extern "C" {

int add(long a, long b) {
    return (int) (a + b);
}

}
