//! -O1

extern int a1, b1;

int sum1() {
    return a1 + b1;
}


extern unsigned a2, b2;

unsigned sum2() {
    return a2 + b2;
}


extern int a3[200];
extern int b3;

int sum3() {
    return a3[0] + b3;
}
