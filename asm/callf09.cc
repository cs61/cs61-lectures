//! -O1

struct point {
    long x;
    long y;
    long z;

    long dimen(int d) const;
};

long point::dimen(int d) const {
    if (d == 0) {
        return this->x;
    } else if (d == 1) {
        return this->y;
    } else {
        return this->z;
    }
}
