CS 61: Data representation 8
============================

Today’s lecture is organized around exercises that test problem set and
lecture concepts.

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

Do the “bad allocations” exercise.

Exercise 1: Bad allocations
---------------------------

Examine the code in `badallocs.cc`. For each of the following questions, your
response should be zero or more lines of code to be placed in `evil_function`.
You shouldn’t add any code anywhere else—no global variables, etc. The problem
set description defines all these terms.

1\. Write code so that the program runs with no memory errors, including no
memory leaks. (`make LSAN=1 badallocs; ./badallocs` can check for leaks and
other errors.)

2\. Write code so that the program runs with memory leaks, but no other
memory errors.

3\. Write code so that the program runs with at least one double-free error.

4\. Write code so that the program runs with at least one use-after-free error.

5\. Write code so that the program runs with at least one wild free error.

6\. Write code so that the program runs with at least one boundary write error.

Exercise 2: Detecting problems without memory accesses
------------------------------------------------------

The problem set asks you to detect many different kinds of problematic frees,
including invalid frees, double frees, wild frees, frees of memory not located
in the heap, and so forth.

1\. Which kinds of problematic frees, if any, could be detected **without
examining memory in the heap or code/data segments** (i.e., only examining
local variables, such as the pointer value passed to `m61-free`)?

2\. Which kinds of problematic frees, if any, could be detected **without
examining memory in the heap segment** (i.e., only considering local and
global variables)?

Exercise 3: Trailers
--------------------

Examine the code in `integertrailer.cc`. Run the program without sanitizers
(`make; ./integertrailer`). Then run it with sanitizers (`make SAN=1;
./integertrailer`). You should see two sanitizer errors.

1\. What C++ rules are violated by this code?

2\. Rewrite `add_trailer` to perform its task without causing sanitizer
errors.

3\. Rewrite `read_trailer` to perform its task without causing sanitizer
errors.

Exercise 4: Discouraging use-after-free
---------------------------------------

This exercise demonstrates different kinds of memory errors, and also makes an
implicit argument about the special qualities of the memory value `0`.

Examine the code in `useafterfree.cc`.

1\. What C++ rules are violated by this program?

Run the program (`make; ./useafterfree`). (Ignore warnings.) You should see
that the rule violations were not detected.

2\. Develop a hypothesis explaining why the violations were not detected.

3\. Change `my_malloc` so that accesses to uninitialized memory are less
likely to go undetected.

4\. Change `my_free` (and perhaps `my_malloc`) so that use-after-free errors
are less likely to remain undetected. (This may require adding metadata to
allocations.)

Exercise 5: malloc_usable_size
------------------------------

Read the manual page for the `malloc_usable_size` function. Modify your
problem set to implement a version of this function.
