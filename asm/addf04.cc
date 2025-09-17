//! -O1 -fcf-protection=none
extern "C" {

int add(char* a, long b) {
    return reinterpret_cast<unsigned long>(&a[b]);
}

}
