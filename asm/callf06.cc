//! -O1
#include <utility>

struct point {
    int x;
    int y;
};

point reflect(point p) {
    return point{p.y, p.x};
}
