extern "C" {

unsigned add(unsigned a, unsigned b) {
    while (b > 0) {
        ++a;
        --b;
    }
    return a;
}

}
