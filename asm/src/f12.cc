//! -O1

extern unsigned short a4, b4;

int sum4() {
    return a4 + b4;
}


extern signed short a5, b5;

int sum5() {
    return a5 + b5;
}


extern unsigned char a6, b6;

int sum6() {
    return a6 + b6;
}


extern long a7, b7;

long sum7() {
    return a7 + b7;
}


extern char* a8;
extern long b8;

char* sum8() {
    return &a8[b8];
}


extern unsigned long long a9;
extern int b9;

int sum9() {
    return (a9 & 0xffff'ffff) + b9;
}
