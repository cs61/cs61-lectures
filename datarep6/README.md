CS 61: Data representation 6
============================

Today’s lecture will start with a discussion of pointers, addresses, and
pointer arithmetic. The advance work involves a toy program `ptrarith.cc` that
defines a structure of type `struct example` and prints its contents using
`hexdump`.

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

1\. How much padding does `struct example` contain, and where is it located?
(This question concerns alignment.)

2\. Write code that defines variables `b4_ptr` and `b8_ptr` that point to the
elements at indexes 4 and 8 of the `e.bytes` array, respectively. What types
do these variables have? Your code should compile without warning or error.

3\. Write code that defines variables `b4_addr` and `b8_addr` that equal the
*addresses* of the elements at indexes 4 and 8 of the `e.bytes` array. The
variables should have type `uintptr_t` (the C/C++ type for addresses). Your
code should compile without warning or error.

4\. Experiment with arithmetic operations on these variables (`+`, `-`, `*`,
`/`, `%`). Which combinations work, and what values do they produce?
