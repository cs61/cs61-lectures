CS 61: Data representation 7
============================

Today’s lecture is primarily about **undefined behavior**. We’ll investigate
undefined behavior through program optimization.

Undefined behavior is a kind of programming error that *no valid program will
ever contain*, but that *programming language implementations need not
detect*. Instead, compilers and other programming languages are allowed to
*assume* that undefined behavior never occurs. The programmer is responsible
for avoiding undefined behavior.

Many programming languages have completely defined behavior. For example, a
Python implementation detects every possible programming error, either
quitting the program when an error occurs or representing the error with a
language construct called an *exception*. Here’s Python handling a
divide-by-zero error. Python, like C and C++, does not allow the programmer to
divide integers by zero.

```python
>>> print(1 / 0);
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ZeroDivisionError: division by zero
```

Every time an integer is divided by zero in Python, the result will be the
same (an exception).

C and C++, on the other hand, have undefined behavior. Here’s a “C++” program
that divides by zero:

```c++
#include <cstdio>
int main() {
    printf("%d\n", 1 / 0);
}
```

Compile this program (`divzero.cc`) with different compilers and options and
it will produce different results.

* CS 61 Docker, compiled with `g++ -O2`: `Illegal instruction` (and program dies).
* CS 61 Docker, compiled with `g++ -O0`: `Floating point exception` (and program dies).
* CS 61 Docker, compiled with `clang++ -O2`: `-529772216`. (Running multiple
  times produces different results: `7431704`, `1599061336`, `-1053791224`, …)

This wide range of output is possible only because the original program **is
not valid C++**. It *looks* like C++, but because it always divides an integer
by zero, the program is invalid, and compilers are allowed to compile invalid
programs so that they do **anything**. That’s very different from Python, and it
makes C and C++ much riskier to program.

C and C++ have undefined behavior for reasons that are in part historical.
Undefined behavior was part of the language before computer scientists
realized how risky it was. It remains part of the language for bad
reasons—such as tradition—and because optimizers can take advantage of
undefined behavior to make programs faster.

The basic rule of optimization is the [**as-if
rule**](https://en.wikipedia.org/wiki/As-if_rule). A compiler can transform a
valid input program however it wants, as long as the transformation preserves
the program’s observable behavior: the transformed program always behaves **as
if** it were the original. The as-if rule lets the compiler remove all kinds
of redundant code. It can throw away functions, remove loops, even replace
slow algorithms with faster ones—anything that keeps the input program’s
behavior the same. But the as-if rule only constrains compilers on *valid*
input programs. If the input program would execute undefined behavior, then
the compiler can do anything.

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

In the first part of the advance work, we’ll investigate how optimizers
improve a valid C++ program using the as-if rule.

1\. Examine the `simplealloc.cc` program. What observable behavior will this
program have (what will it do when it’s run)?

2\. Compile the `simplealloc.cc` program using the `clang++` compiler, with
and without optimization: `make simplealloc simplealloc-O0`. (Our makefile for
this directory chooses `clang++` by default.) Run the program
\(`./simplealloc`, `./simplealloc-O0`) and check your understanding.

3\. Examine the instructions in `./simplealloc-O0` (which is
unoptimized) using this command line:

```sh
$ objdump --disassemble=main -C simplealloc-O0
```

Look especially for the `callq` instruction, which is used to call
functions. How many function calls are there? To what functions?

4\. Examine the instructions in `./simplealloc` (which is optimized)
using this command line:

```sh
$ objdump --disassemble=main -C simplealloc
```

5\. Using these observations, what are **two** optimizing transformations that
the compiler can perform based on the as-if rule?

In the second part of the advance work, we’ll turn to a program that has
undefined behavior.

6\. Examine the `ubsignedinc.cc` program. What observable behavior will this
program have (what will it do when it’s run)?

7\. Compare the results of `./ubsignedinc-O0 0x7fffffff 1` and `./ubsignedinc
0x7fffffff 1`.

8\. Using these observations, what is a behavior exhibited by `ubsignedinc`
that is undefined in C++?
