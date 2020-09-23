int fun(const char* s) {
    if (s[0] != 0
        && s[0] == s[1]
        && s[1] != 0) {
        return s[2];
    } else {
        return -1;
    }
}
