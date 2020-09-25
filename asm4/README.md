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

PreWork
------------
There is no prework in this lecture.

Postwork
------------

*This work is not required. If you are curious about our actual implementation we discussed in class, you can read the code and answer the following questions.*

1\. How can we represent real numbers using integers? Which way is more accurate?

2\. How can we support multiplication? Log? and Exp?

3\. How would the arithmetic accuracy change if we increase the number of entries in _log_lookup_table and _exp_lookup_table?

You can also check out the following links if you are curious about data centers and seeing more real switches:
- Virtual tour of Google datacenter
https://www.youtube.com/watch?v=XZmGGAbHqa0&feature=emb_logo

- Here’s the facebook one:
https://www.youtube.com/watch?v=7wQHs2G5hPA

- New york times article:
https://www.nytimes.com/2009/06/14/magazine/14search-t.html
