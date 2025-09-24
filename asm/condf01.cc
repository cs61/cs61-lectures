//! -O3

int calc1(int input, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += input;
        input = input * 1003 + n;
    }
    return sum;
}
