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


extern signed short a4, b4;

int sum4() {
    return a4 + b4;
}


extern unsigned char a5, b5;

int sum5() {
    return a5 + b5;
}


extern long a6, b6;

long sum6() {
    return a6 + b6;
}


extern char* a7;
extern long b7;

char* sum7() {
    return &a7[b7];
}


extern unsigned long long a8;
extern int b8;

int sum8() {
    return (a8 & 0xffff'ffff) + b8;
}
