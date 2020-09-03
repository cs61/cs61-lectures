CS61 2020: Data Representation 2
================================

This directory contains the source code for several simple programs that print
the contents of memory. They don’t take arguments. To run them:

```shellsession
$ make
...
$ ./mexplore0
... prints output ...
$ ./mexplore1
... prints output ...
```

This lecture will discuss how a program’s memory is laid out—its *address
space*. We’ll name different regions, or *segments*, of memory, position them
relative to each other, and describe their behavior. Different kinds of
objects are located in different segments; we’ll discuss how object lifetimes
relate to segments.

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

1\. Read the `mexplore0.cc` and `mexplore1.cc` files and try to understand
them. Bring questions about anything you don’t understand. Develop an idea,
before running the code, of how it will behave.

2\. Run the `./mexplore0` and `./mexplore1` programs. Each program produces
more than one line of output. For each program, how do its output lines
relate? Think about similarities and differences.
