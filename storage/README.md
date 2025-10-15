CS 61 2021: Storage 1
=====================

The `w*` programs test the speed of `w`riting data to disk using different
operating system and library interfaces. Each program writes the same data,
but:

* The `byte` versions write one byte at a time.
* The `block` versions write 512 bytes at a time.
* The `sync` versions make one system call per write, and ask the OS to write
  data to stable storage (i.e., the attached disk drive) before returning.
  This prevents the OS from speeding up the system calls using its file cache
  system.
* The `os` versions make one system call per write, but allow caching.
* The `stdio` versions make one *library function call* per write, to the
  standard I/O library.

So here are six programs: `./w-syncbyte`, `./w-syncblock`, `./w-osbyte`,
`./w-osblock`, `./w-stdiobyte`, `./w-stdioblock`.

1\. Run these programs. They report how fast they go. What do you observe
about their speeds, both absolute and relative to each other?

2\. (Optional) If you have access to multiple computers or operating systems
(for instance, Docker plus the host OS, or a server), run the programs in
several contexts. Do their speeds and speed relationships change?

> **Note.** The `w` programs have command line arguments.
>
> * `-s SIZE`: Write `SIZE` bytes total (rather than 5120000 or whatever).
> * `-b BLOCKSIZE` (`block` programs only): Write blocks of `BLOCKSIZE` bytes
>   (rather than 512).
> * `-q`: Print a single speed report at the end of the run, rather than
>   continuously updating the speed report.
