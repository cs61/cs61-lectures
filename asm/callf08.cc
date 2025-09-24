//! -O1

struct point {
    long x;
    long y;
    long z;
};

point rotate(point p) {
    return point{p.y, p.z, p.x};
}
