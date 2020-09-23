//! -O1 -fno-stack-protector
// Let's figure out the calling conventions on this compiler!

int process_array(const int* first, const int* last);

void f(long arg1, long arg2, long arg3, long arg4,
       long arg5, long arg6) {
    int a[6];
    a[0] = arg1;
    a[1] = arg2;
    a[2] = arg3;
    a[3] = arg4;
    a[4] = arg5;
    a[5] = arg6;
    process_array(&a[0], &a[6]);
}
