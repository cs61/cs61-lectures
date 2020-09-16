CS 61: Data representation 9
============================

Today’s lecture explores a particularly nasty kind of undefined behavior, a
**buffer overflow attack**. The example pulls together aspects of memory
layout as well as undefined behavior.

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

We’ll focus on a program called `attackme.cc` that is ripe for attacking.

The `attackme` program computes a **checksum** of its input. A checksum is a
function that takes a string and returns an integer. Like a hash function, a
checksum must have the property that the checksums of equal strings are equal,
and checksums of different strings should usually be different. We use
checksums in computer science to validate data. For instance, if Alice wants
to transmit a message to Bob, then Alice can send both the message and a
checksum of the message. When Bob receives the message, Bob will compute the
checksum on his version, then compare it with Alice’s checksum. If the
checksums differ, then Bob knows that the message was corrupted.

There’s a whole branch of computer science theory about creating good checksum
functions. The best checksums, **cryptographic hash functions**, have the
property that no one knows how to efficiently find two strings with the same
checksum. You may have heard of some of these, such as [SHA-256][] and
[SHA-1][] (which is now considered insecure).

`attackme` computes a **non-cryptographic** checksum. These are not secure,
but they still can be useful. Run `./attackme` with some arguments, such as
`./attackme Hello world`, to see the checksums it creates. To compute a
checksum of the standard input, use a command line such as `echo Hello world |
./attackme`.

1\. Examine the `attackme.cc` code. Describe in your own words how `attackme`
computes checksums.

2\. Sketch a picture of `attackme`’s memory at the point that the `checksum`
function returns. Which variables are located in which segments of memory? Use
`hexdump` to print addresses if you’re unsure.

3\. Is there any input for the `attackme` program that will cause it to call
the `exec_shell` function?

[SHA-1]: https://en.wikipedia.org/wiki/SHA-1
[SHA-256]: https://en.wikipedia.org/wiki/SHA-2
