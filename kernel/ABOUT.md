WeensyOS
========

This is WeensyOS, a teaching operating system built for Harvard’s
[CS 61].

Quickstart: `make run` will run the OS using the [QEMU] emulator.

Make targets
------------

`make run` will run the OS. On Docker, the OS will run in your terminal
window; on a Linux virtual machine, QEMU will open a new window. (`make
run-console` will run the OS in the terminal window on any OS.)

To exit the OS, type `q` into the terminal or QEMU window. Since this requires
a semi-functioning kernel, it might not work for you; see
[Troubleshooting](#troubleshooting) for other ways to kill the OS.

WeensyOS creates a debug log in `log.txt`. Run `make LOG=stdio run` to
redirect the debug log to the standard output, or `make
LOG=file:FILENAME run` to redirect it to `FILENAME`.

`make D=1 run` tells QEMU to print verbose information about interrupts and
CPU resets to the file `qemu.log`. This setting will also cause QEMU to quit
after encountering a [triple fault][] (normally it will reboot).

`make run-PROGRAM` runs `p-PROGRAM.cc` as the first non-init process. The
default is `allocators`, which runs four copies of `p-allocator.cc`.

Finally, `make clean` cleans up your directory.

Building
--------

**Linux:** WeensyOS should build natively on a Linux machine or
virtual machine. `qemu` packages are required to run WeensyOS; on
Ubuntu, `sudo apt install qemu qemu-system-x86` should work. A recent
compiler is required. You can use [Clang](https://clang.llvm.org/),
but only version 5 or later.

**Mac OS X:** WeensyOS can build on Mac OS X after some tools are installed.

1. Install [Homebrew].

2. Install Homebrew’s new GCC package: `brew install gcc`

3. Install Homebrew’s QEMU: `brew install qemu`

4. Install a collection of cross-compilers for x86-64 Linux. At time of
   writing, [this
   collection](https://github.com/messense/homebrew-macos-cross-toolchains):
   works: `brew tap messense/homebrew-macos-cross-toolchains; brew install
   x86_64-unknown-linux-gnu`

5. Create a file `config.mk` in this directory containing this:

    ```make
    CCPREFIX=x86_64-unknown-linux-gnu-
    HOSTCC=gcc-14
    HOSTCXX=g++-14
    ```

    But do not add `config.mk` to your repository! It configures the
    build for your working directory environment, and the grading
    server (for instance) can’t use those settings.

Troubleshooting
---------------

There are several ways to kill a recalcitrant WeensyOS (if, for instance, your
OS has become unresponsive).

* If QEMU is running in its own graphical window, then close the window. This
  will kill the embedded OS.

* Open another terminal, change into the WeensyOS directory, and run `make
  stop` to stop all running QEMUs. If you are using Docker, you will need to
  enter Docker via `../cs61-run-docker` before running `make stop`.

* If QEMU is running in a terminal window (in Docker, for instance), then
  press `Alt-2` (or `Option-2`). This will bring up the QEMU Monitor, which
  looks like this:

    ```
    compat_monitor0 console
    QEMU 4.2.0 monitor - type 'help' for more information
    (qemu)
    ```

    Type `quit` and hit Return to kill the embedded OS and return to your
    shell. If this leaves the terminal looking funny, enter the `reset` shell
    command to restore it.

    If `Alt-2` does not work, you may need to configure your terminal to
    properly send the Alt key. For instance, on Mac OS X’s Terminal, go to the
    Edit menu, select “Use Option as Meta key”, and press `Option-2`. You can
    also configure a special keyboard shortcut that sends the `Escape 2`
    sequence.

Run `make run-gdb` to start up the OS with support for GDB debugging.
This will start the OS, but not GDB. You must run `gdb -ix
build/weensyos.gdb` to connect to the running emulator; when GDB
connects, it will stop the OS and wait for instructions.

If you experience runtime errors involving `obj/libqemu-nograb.so.1`, put
`QEMU_PRELOAD_LIBRARY=` in `config.mk`. This disables a shim we use that
prevents QEMU from grabbing the mouse.

Source files
------------

Real operating systems are big. We have tried to boil down this OS to
a minimum, comment it to help you, and separate x86-64 specifics from
more fundamental issues.

### Common files

| File            | Description                            |
| --------------- | -------------------------------------- |
| `types.h`       | Type definitions                       |
| `lib.hh/cc`     | C library                              |
| `x86-64.h`      | x86-64 hardware definitions            |
| `elf.h`         | ELF64 structures for loading programs  |

### Boot loader

| File             | Description                  |
| ---------------- | ---------------------------- |
| `bootentry.S`    | Boot loader entry point      |
| `boot.cc`        | Boot loader main code        |
| `build/boot.ld`  | Boot loader linker script    |

### Kernel core

| File                | Description                          |
| ------------------- | ------------------------------------ |
| `kernel.hh`         | Kernel declarations                  |
| `k-exception.S`     | Kernel entry points                  |
| `k-hardware.cc`     | Kernel initialization and hardware   |
| `k-vmiter.hh/cc`    | Page table iterators                 |
| `kernel.cc`         | Kernel exception handlers            |
| `k-memviewer.cc`    | Kernel memory viewer                 |
| `build/kernel.ld`   | Kernel linker script                 |

### Kernel libraries

| File                | Description                          |
| ------------------- | ------------------------------------ |
| `k-apic.hh`         | Interrupt controller hardware        |
| `k-pci.hh`          | PCI bus hardware                     |

### Processes

| File               | Description                                      |
| ------------------ | ------------------------------------------------ |
| `u-lib.cc/hh`      | Process library and system call implementations  |
| `p-*.cc`           | Process code                                     |
| `build/process.ld` | Process binary linker script                     |

Build files
-----------

The main output of the build process is a disk image,
`weensyos.img`. QEMU “boots” off this disk image, but the image
could conceivably boot on real hardware! The build process also
produces other files that can be useful to examine.

| File                       | Description                          |
| -------------------------- | ------------------------------------ |
| `obj/kernel.asm`           | Kernel assembly (with addresses)     |
| `obj/kernel.sym`           | Kernel defined symbols               |
| `obj/p-PROCESS.asm`, `sym` | Same for process binaries            |

[triple fault]: https://en.wikipedia.org/wiki/Triple_fault
[CS 61]: https://cs61.seas.harvard.edu/
[QEMU]: https://qemu.org/
[Homebrew]: https://brew.sh/
