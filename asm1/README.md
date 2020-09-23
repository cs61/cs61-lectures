CS61 2020: Assembly 1
=====================

This unit will teach you much more about machine code—the binary instructions
that the processor interprets—and assembly language—the human-readable form of
machine code.

In lecture we’ll spend time reverse-engineering assembly language files,
showing that the behavior of assembly language can in some ways be inferred.
If you prefer a more straightforward presentation, try the textbook and the
[lecture notes](https://cs61.seas.harvard.edu/site/2020/Asm).

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

Many aspects of assembly language can be inferred by writing C++ code, running
the compiler, and examining the results using `objdump -d`.

1\. Write or sketch C++ functions that would explore how values are returned
from functions in x86-64 assembly language. Might functions represent the
returns of different kinds of objects in different ways? Why or why not?

2\. Write or sketch C++ functions that would explore how arrays are
represented in x86-64 assembly language. Might different kinds of array
manipulations be represented in different ways? Why or why not?

In case you want to explore, the `GNUmakefile` in this directory may help. If
you create a file in this directory called `fXXXX.cc` (for any `XXXX`), then:

* Run `make fXXXX.asm` and the compiler will compile the input file and then
  disassemble the resulting object file into `fXXXX.asm`.
* Run `make fXXXX.s` and the compiler will generate assembly output in
  `fXXXX.s` directly.

Both methods generate the same *instructions*, but the `fXXXX.s` output will
be longer. `fXXXX.s` contains verbose compiler-generated annotations and
labels; these can aid understanding, but they often get in the way.
