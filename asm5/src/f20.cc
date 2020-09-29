//! -O2 -fno-stack-protector

// Things to try:
// - Add members to `s1` and/or `s2`
// - Change types of members in `s1` and/or `s2`

struct s1 {
    int first = 0;
    int second = 0;
};

struct s2 {
    int first = 0;
    int second = 0;
};

s2 f(s1 arg1, int arg2, s1 arg3) {
    s2 ret;
    ret.second = arg1.first + arg2 + arg3.second;
    return ret;
}
