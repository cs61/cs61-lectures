//! -O1

struct pr {
    int first;
    int second;
    char c[30];
};

int array_access(pr* a, long i) {
    return a[i].second;
}
