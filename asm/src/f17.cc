//! -O1

struct pr {
    int first;
    int second;
};

int array_access(pr* a, long i) {
    return a[i].first;
}
