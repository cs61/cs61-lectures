CS61 2020: Data Representation 3
================================

This directory contains the source code for programs that print the contents
of different C++ **collections**. They don’t take arguments. To run them:

```shellsession
$ make
...
$ ./array0      # or ./array1, ./struct0, ./struct1, ./struct2, ./struct3
... prints output ...
```

Collections are ways of creating new C++ types from components. C++ offers
three important kinds of collection: **structs**, **arrays**, and **unions**.
We focus mostly on structs and arrays. A struct comprises named components
that may have different types. An array comprises *numbered* components that
must all have the same type.

This lecture will discuss how collections are laid out in memory. Different
kinds of collections are laid out differently.

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

1\. Read the `array*.cc` and `struct*.cc` files and try to understand them.
Bring questions about anything you don’t understand. Develop an idea, before
running the code, of how it will behave.

2\. Run the programs, including `./array0` and `./array1` and the four
`./struct*` programs. Which `./array*` programs match which `./struct*`
programs? Can you develop a theory of why?

3\. If you run a program multiple times, which parts of its output change, if
any?

4\. How does the size of a collection relate to the sizes of its components?
Develop a hypothesis, if you can.
