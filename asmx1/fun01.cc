#include <string.h>
#include <stdio.h>

int __attribute__ ((noinline)) frog(char* input, int f) {
  if ((int) strlen(input) < f) {
    return (int) strlen(input);
  }
  else {
    return f;
  }
}

int __attribute__ ((noinline)) strange(int a, int b, int c, int d, char*input, int f) {
  int x = a + b + c + d;
  if (x < f) {
    return x;
  }
  return frog(input, f);
}

int __attribute__ ((noinline)) nonsense(int a, int b, int c, int d, char* input, char* output, int f) {
  int x = strange(a, b, c, d, input, f);
  if (x == f) {
    return f;
  }
  printf("%s\n", output);
  return 0;
}

int main() {
  const char* input = "hello";
  const char* output = "goodbye";
  nonsense(1, 2, 3, 4, (char*) input, (char*) output, 8);
} 
  
