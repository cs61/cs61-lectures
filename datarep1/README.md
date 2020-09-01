CS61 2020: Data Representation 1
================================

This directory contains the source code for a simple program, `add`.
The `add` program reads two integers from its command-line arguments
and prints their sum. To build and run it:

```shellsession
$ make add
c++  -std=gnu++1z -W -Wall -Wshadow -g   -MD -MF .deps/add.d -MP -O3 -o add.o -c add.cc
c++  -std=gnu++1z -W -Wall -Wshadow -g   -MD -MF .deps/addf.d -MP -O3 -fomit-frame-pointer -o addf.o -c addf.cc
c++ -std=gnu++1z -W -Wall -Wshadow -g    -O3 -o add add.o addf.o
$ ./add 1 2
1 + 2 = 3
```

The source code is stored in two files, `add.cc` and `addf.cc`. `addf.cc`
contains the `add` function, defined in the natural way; `add.cc` contains
`main`, the program entry point that calls `add`. Then there is `GNUmakefile`,
which contains rules for building the `add` program from those source files.

> **About shell sessions.** A shell session, like the short snippet above,
> both shows commands you could type and the output of those commands. By
> convention, the commands you type are shown on lines that start with a shell
> prompt, which is usually `$`, `%`, or a short string ending with `$` or `%`.
> You won’t type the shell prompt.

Entry point
-----------

Every program has an entry point, which is a function that’s called when the
program starts running. In C and C++ the function is called `main` and its
parameters are `int argc, char* argv[]`. The `argc` parameter equals the
number of command-line arguments passed to the program, including the program
name, and the `argv` parameter is an array of strings defining those
arguments.

Every programming language defines its entry point in its own way. In Java,
the entry point is a `static public main(String[] args)` method on some class.
In a scripting language like Python, the entry point is the top of the script.

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

1\. Read the `addf.cc` and `add.cc` files and try to understand them. Bring
questions about anything you don’t understand.

Some links:
* C++ library function [strtol](https://en.cppreference.com/w/cpp/string/byte/strtol)
    * [Linux manual page](https://man7.org/linux/man-pages/man3/strtol.3.html)
    * [Tutorial](https://www.tutorialspoint.com/c_standard_library/c_function_strtol.htm)
* C++ library function [printf](https://en.cppreference.com/w/cpp/io/c/fprintf)
    * [Linux manual page](https://man7.org/linux/man-pages/man3/printf.3.html)
    * [Example tutorial](https://www.cprogramming.com/tutorial/printf-format-strings.html)

2\. Are there any alternate implementations of the `add` function that produce
the exact same outputs for all inputs, but work differently? Think of as many
as you can.
