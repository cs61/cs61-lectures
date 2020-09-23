CS61 2020: Data Representation at Switches
================================

This directory contains the source code for data representations at switches

```shellsession
$ make
...
$ ./switch-bit 4.2 3.7
... prints output ...
```
This lecture will describe how to do arithmetics of real numbers on a switch that only supports fixed point types but floating points.
The switch only supports addition, deletion, all kinds of bit operations, and table lookups,  but not multiplication, log, and exp. 

Advance work
------------

*Advance work should be completed before lecture, especially by active
listeners. There is nothing to turn in.*

1\. Read the `switch-bit.cc` files and try to understand
them. Bring questions about anything you don’t understand.

2\. How can we represent real numbers using integers? Which way is more accurate?

3\. How can we support multiplication? Log? and Exp?

4\. How would the arithmetic accuracy change if we increase the number of entries in _log_lookup_table and _exp_lookup_table?
