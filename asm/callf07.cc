//! -O2
#include <utility>

struct point {
    long x;
    long y;
};

point reflect(point p) {
    return point{p.y, p.x};
}
