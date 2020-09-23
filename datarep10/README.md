CS 61: Data representation 10
=============================

Today’s lecture is about computer arithmetic.

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

Binary unsigned integers are represented as bit sequences, or
sequences of binary digits. For example, the 8-bit unsigned integer
`b` might be written:

```
    +---+---+---+---+---+---+---+---+
b = |b_7|b_6|b_5|b_4|b_3|b_2|b_1|b_0|
    +---+---+---+---+---+---+---+---+
```

where each `b_i` is either 0 or 1. The least-order bit is written on
the right, as in our Arabic numeral system, so bit `b_i` has place
value 2<sup><code>i</code></sup>.

Signed integers are also represented as sequences of bits, but the
values of some of the bits are different.

1\. Define a mathematical function `lowestbit(b)` that takes an
unsigned integer `b` and returns the value of `b_0`. Write the function
**without** using bitwise operators. (Hint: Consider using the modulus
operator. You can write in C if you prefer.)

2\. Define a function `bit(b, i)` that returns the value of `b_i`.
Again, do not use bitwise operators.

3\. (Optional) Assume `b` is an unsigned integer with `N` bits, and
`x` is the integer part of `b/8`. Define `x_i` (for 0≤`i`<`N`) in
terms of the bits of `b`.
