CS 61: Data representation 4
============================

Type `make` to build the programs in this directory.

`arraysum`
----------

The `arraysum` program tests how long it takes to add up all elements in an
array of 4-byte integers. The inner timing loop of each run accesses exactly
the same amount of data; the only difference between these runs is the order
in which the array elements are accessed. Try:

```sh
$ ./arraysum -u 100000     # accesses 100,000 integers in “up” order:
                           # index 0, then index 1, then index 2, etc.
$ ./arraysum -d 100000     # accesses 100,000 integers in “down” order:
                           # index 99,999, then index 99,998, etc.
$ ./arraysum -r 100000     # accesses 100,000 integers in random order
$ ./arraysum -m 100000     # accesses 100,000 integers in “magic” order
                           # (bouncing between the ends of the array)
```

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

1\. Read the `arraysum.cc` file and try to understand it. Bring questions
about anything you don’t understand. Develop an idea, before running the code,
of how it should behave. Do you expect different access orders to take
different amounts of time?

2\. Run the programs. Which programs run fastest? Which run slowest? Can you
develop a theory of why?
