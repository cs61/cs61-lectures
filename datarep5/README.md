CS 61: Data representation 5
============================

Type `make` to build the programs in this directory, which are variants of a
program called `membench` or `mb`. These programs tests the speed of memory
allocation, and specifically of the allocation and freeing of many small
objects called **memnodes**. Run one of these programs like this:

```sh
$ time ./mb0
nops 10000000, nthreads 1, answer 40968386560

real    0m0.832s
user    0m0.826s
sys     0m0.002s
```

The benchmark code lives in `mb0.cc` through `mb5.cc`, in a function called
`memnode_benchmark`. Here’s what that function does:

1. It allocates 4096 memnodes into an array. (A memnode is a simple structure
   containing a string “name” and an integer “number”.)
2. It runs 10,000,000 operations, where each operation selects a memnode,
   frees that memnode, and then allocates another memnode. This heavily
   stresses the memory allocator.
3. It frees the remaining memnodes and returns a computed result.

Memnodes are allocated and freed by calling special `allocate()` and
`deallocate()` member functions on an object called an **arena**. In the first
programs, these functions are a simple wrapper around `new` and `delete`
(which, in turn, are wrappers around `malloc` and `free`). We will speed up
the memory benchmark by roughly 30x by adding features to these functions.

(The term “arena allocation” is used in [region-based memory
management](https://en.wikipedia.org/wiki/Region-based_memory_management).)

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

1\. Read the `mb0.cc` file and try to understand it. Bring questions about
anything you don’t understand. Develop an idea, before running the code, of
how it should behave.

2\. Modify `mb0.cc` to print out the memory contents of two or three different
memnodes in the array using `hexdump` or `hexdump_object`. What is a memnode’s
size? How many allocations does it contain? Where are the characters
comprising the name located?

3\. Draw a picture of the objects in the heap and the stack. Include two or
three memnodes. Say which objects are located in the stack and which are in
the heap.
