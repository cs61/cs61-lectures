int fun(const char* s) {
    if (s[0] == 0) {
        return -1;
    }
    while (true) {
        if (s[0] != s[1]) {
            return s[1];
        }
        ++s;
    }
}
