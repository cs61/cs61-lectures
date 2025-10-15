#include "kernel.hh"
#include "k-apic.hh"
#include "k-vmiter.hh"
#include "obj/k-firstprocess.h"
#include <atomic>

// kernel.cc
//
//    This is the kernel.


// INITIAL PHYSICAL MEMORY LAYOUT
//
//  +-------------- Base Memory --------------+
//  v                                         v
// +-----+--------------------+----------------+--------------------+---------/
// |     | Kernel      Kernel |       :    I/O | App 1        App 1 | App 2
// |     | Code + Data  Stack |  ...  : Memory | Code + Data  Stack | Code ...
// +-----+--------------------+----------------+--------------------+---------/
// 0  0x40000              0x80000 0xA0000 0x100000             0x140000
//                                             ^
//                                             | \___ PROC_SIZE ___/
//                                      PROC_START_ADDR

#define PROC_SIZE 0x40000       // initial state only

proc ptable[MAXNPROC];          // array of process descriptors
                                // Note that `ptable[0]` is never used.
proc* current;                  // pointer to currently executing proc

#define HZ 100                  // timer interrupt frequency (interrupts/sec)
[[maybe_unused]] std::atomic<unsigned long> ticks; // # timer interrupts so far


// Memory state - see `kernel.hh`
physpageinfo physpages[NPAGES];


[[noreturn]] void schedule();
[[noreturn]] void run(proc* p);
void exception(regstate* regs);
uintptr_t syscall(regstate* regs);


// kernel_start(command)
//    Initialize the hardware and processes and start running. The `command`
//    string is an optional string passed from the boot loader.

static void process_setup(pid_t pid, const char* program_name);

void kernel_start(const char* command) {
    // initialize hardware
    init_hardware();
    log_printf("Starting WeensyOS\n");

    ticks = 1;

    // clear screen
    console_clear();

    // (re-)initialize kernel page table
    vmiter it(kernel_pagetable, 0);
    for (; it.va() < MEMSIZE_PHYSICAL; it += PAGESIZE) {
        uintptr_t addr = it.va();
        int perm = PTE_P | PTE_W | PTE_U;
        if (addr == 0) {
            // nullptr is inaccessible even to the kernel
            perm = 0;
        }
        // install identity mapping
        int r = it.try_map(addr, perm);
        assert(r == 0); // mappings during kernel_start MUST NOT fail
                        // (Note that later mappings might fail!!)
    }

    // set up process descriptors
    for (pid_t i = 0; i < MAXNPROC; i++) {
        ptable[i].pid = i;
        ptable[i].state = P_FREE;
    }
    if (!command) {
        command = WEENSYOS_FIRST_PROCESS;
    }
    if (!program_image(command).empty()) {
        process_setup(1, command);
    } else if (strcmp(command, "pipe") == 0) {
        process_setup(1, "pipewriter");
        process_setup(2, "pipereader");
    } else {
        process_setup(1, "alice");
        process_setup(2, "eve");
    }

    // switch to first process using run()
    run(&ptable[1]);
}


// process_setup(pid, program_name)
//    Load application program `program_name` as process number `pid`.
//    This loads the application's code and data into memory, sets its
//    %rip and %rsp, gives it a stack page, and marks it as runnable.

void process_setup(pid_t pid, const char* program_name) {
    init_process(&ptable[pid], 0);

    // We expect all process memory to reside between
    // first_addr and last_addr.
    uintptr_t first_addr = PROC_START_ADDR + (pid - 1) * PROC_SIZE;
    uintptr_t last_addr = PROC_START_ADDR + pid * PROC_SIZE;

    // initialize process page table
    ptable[pid].pagetable = kernel_pagetable;

    // obtain reference to program image
    // (The program image models the process executable.)
    program_image pgm(program_name);

    // allocate process memory as specified in program image
    for (auto seg = pgm.begin(); seg != pgm.end(); ++seg) {
        for (uintptr_t a = round_down(seg.va(), PAGESIZE);
             a < seg.va() + seg.size();
             a += PAGESIZE) {
            // `a` is the process virtual address for the next code or data page
            assert(a >= first_addr && a < last_addr);
            assert(physpages[a / PAGESIZE].refcount == 0);
            ++physpages[a / PAGESIZE].refcount;
        }
    }

    // copy instructions and data from program image into process memory
    for (auto seg = pgm.begin(); seg != pgm.end(); ++seg) {
        memset(reinterpret_cast<void*>(seg.va()), 0, seg.size());
        memcpy(reinterpret_cast<void*>(seg.va()), seg.data(), seg.data_size());
    }

    // mark entry point
    ptable[pid].regs.reg_rip = pgm.entry();

    // allocate stack segment
    uintptr_t stack_addr = last_addr - PAGESIZE;
    assert(physpages[stack_addr / PAGESIZE].refcount == 0);
    ++physpages[stack_addr / PAGESIZE].refcount;
    ptable[pid].regs.reg_rsp = stack_addr + PAGESIZE;

    // allow process to control interrupts
    ptable[pid].regs.reg_rflags |= EFLAGS_IOPL_3;

    // mark process as runnable
    ptable[pid].state = P_RUNNABLE;
}



// exception(regs)
//    Exception handler (for interrupts, traps, and faults).
//
//    The register values from exception time are stored in `regs`.
//    The processor responds to an exception by saving application state on
//    the kernel's stack, then jumping to kernel assembly code (in
//    k-exception.S). That code saves more registers on the kernel's stack,
//    then calls exception().
//
//    Note that hardware interrupts are disabled when the kernel is running.

void exception(regstate* regs) {
    // Copy the saved registers into the `current` process descriptor.
    current->regs = *regs;
    regs = &current->regs;

    // It can be useful to log events using `log_printf`.
    // Events logged this way are stored in the host's `log.txt` file.
    /* log_printf("proc %d: exception %d at rip %p\n",
                current->pid, regs->reg_intno, regs->reg_rip); */

    // Show the current cursor location.
    console_show_cursor();

    // If Control-C was typed, exit the virtual machine.
    check_keyboard();


    // Actually handle the exception.
    switch (regs->reg_intno) {

    case INT_PF: {
        // Analyze faulting address and access type.
        uintptr_t addr = rdcr2();
        const char* operation = regs->reg_errcode & PTE_W
                ? "write" : "read";
        const char* problem = regs->reg_errcode & PTE_P
                ? "protection problem" : "missing page";

        if (!(regs->reg_errcode & PTE_U)) {
            proc_panic(current, "Kernel page fault on %p (%s %s, rip=%p)!\n",
                       addr, operation, problem, regs->reg_rip);
        }
        error_printf("Process %d page fault on %p (%s %s, rip=%p)!\n",
            current->pid, addr, operation, problem, regs->reg_rip);
        goto unhandled;
    }

    case INT_GP: {
        const char* entity = regs->reg_cs & 3 ? "Process" : "Kernel";

        error_printf("%s general protection fault (rip=%p)!\n",
            entity, regs->reg_rip);
        goto unhandled;
    }

    default:
    unhandled:
        proc_panic(current, "Unhandled exception %d (rip=%p)!\n",
                   regs->reg_intno, regs->reg_rip);

    }


    // Return to the current process (or run something else).
    if (current->state == P_RUNNABLE) {
        run(current);
    } else {
        schedule();
    }
}


// syscall(regs)
//    Handle a system call initiated by a `syscall` instruction.
//    The process’s register values at system call time are accessible in
//    `regs`.
//
//    If this function returns with value `V`, then the user process will
//    resume with `V` stored in `%rax` (so the system call effectively
//    returns `V`). Alternately, the kernel can exit this function by
//    calling `schedule()`, perhaps after storing the eventual system call
//    return value in `current->regs.reg_rax`.
//
//    It is only valid to return from this function if
//    `current->state == P_RUNNABLE`.
//
//    Note that hardware interrupts are disabled when the kernel is running.

int syscall_pipewritec(int ch);
int syscall_pipereadc();
ssize_t syscall_pipewrite(const char* buf, size_t sz);
ssize_t syscall_piperead(char* buf, size_t sz);

uintptr_t syscall(regstate* regs) {
    // Copy the saved registers into the `current` process descriptor.
    current->regs = *regs;
    regs = &current->regs;

    // It can be useful to log events using `log_printf`.
    // Events logged this way are stored in the host's `log.txt` file.
    /* log_printf("proc %d: syscall %d at rip %p\n",
                  current->pid, regs->reg_rax, regs->reg_rip); */

    // Show the current cursor location.
    console_show_cursor();

    // If Control-C was typed, exit the virtual machine.
    check_keyboard();


    // Actually handle the exception.
    switch (regs->reg_rax) {

    case SYSCALL_PANIC:
        user_panic(current);
        break; // will not be reached

    case SYSCALL_GETPID:
        return current->pid;

    case SYSCALL_YIELD:
        current->regs.reg_rax = 0;
        schedule();             // does not return

    case SYSCALL_WRITE: {
        const char* buf = (const char*) current->regs.reg_rdi;
        size_t sz = current->regs.reg_rsi;
        console_puts(-1, 0x0700, buf, sz);
        return sz;
    }

    case SYSCALL_GETSYSNAME: {
        const char* osname = "DemoOS 61.61";
        char* buf = (char*) current->regs.reg_rdi;
        strcpy(buf, osname);
        return 0;
    }

    case SYSCALL_PIPEWRITEC:
        return syscall_pipewritec(current->regs.reg_rdi);

    case SYSCALL_PIPEREADC:
        return syscall_pipereadc();

    case SYSCALL_PIPEWRITE:
        return syscall_pipewrite(
            reinterpret_cast<const char*>(current->regs.reg_rdi),
            current->regs.reg_rsi
        );

    case SYSCALL_PIPEREAD:
        return syscall_piperead(
            reinterpret_cast<char*>(current->regs.reg_rdi),
            current->regs.reg_rsi
        );

    default:
        proc_panic(current, "Unhandled system call %ld (pid=%d, rip=%p)!\n",
                   regs->reg_rax, current->pid, regs->reg_rip);

    }

    panic("Should not get here!\n");
}


// pipe buffer

char pipebuf[1];
size_t pipebuf_len = 0;

int syscall_pipewritec(int ch) {
    // See `sys_pipewritec` in `u-lib.cc` for specification.
    if (ch < 0 || ch > 255) {
        return E_INVAL;
    } else if (pipebuf_len == 1) {
        // kernel buffer full, process should try again
        return E_AGAIN;
    }
    // write byte
    pipebuf[0] = ch;
    pipebuf_len = 1;
    return 1;
}

int syscall_pipereadc() {
    // See `sys_pipereadc` in `u-lib.cc` for specification.
    if (pipebuf_len == 0) {
        // kernel buffer empty, process should try again
        return E_AGAIN;
    }
    // read one byte
    pipebuf_len = 0;
    return static_cast<unsigned char>(pipebuf[0]);
}

ssize_t syscall_pipewrite(const char* buf, size_t sz) {
    // See `sys_pipewrite` in `u-lib.cc` for specification.
    if (sz == 0) {
        // nothing to write
        return 0;
    } else if (pipebuf_len == 1) {
        // kernel buffer full, process should try again
        return E_AGAIN;
    }
    // write one byte
    pipebuf[0] = buf[0];
    pipebuf_len = 1;
    return 1;
}

ssize_t syscall_piperead(char* buf, size_t sz) {
    // See `sys_piperead` in `u-lib.cc` for specification.
    if (sz == 0) {
        // no room to read
        return 0;
    } else if (pipebuf_len == 0) {
        // kernel buffer empty, process should try again
        return E_AGAIN;
    }
    // read one byte
    buf[0] = pipebuf[0];
    pipebuf_len = 0;
    return 1;
}


// schedule
//    Pick the next process to run and then run it.
//    If there are no runnable processes, spins forever.

void schedule() {
    pid_t pid = current->pid;
    for (unsigned spins = 1; true; ++spins) {
        pid = (pid + 1) % MAXNPROC;
        if (ptable[pid].state == P_RUNNABLE) {
            run(&ptable[pid]);
        }
        if (spins > MAXNPROC) {
            panic("No runnable processes!");
        }

        // If Control-C was typed, exit the virtual machine.
        check_keyboard();
    }
}


// run(p)
//    Run process `p`. This involves setting `current = p` and calling
//    `exception_return` to restore its page table and registers.

void run(proc* p) {
    assert(p->state == P_RUNNABLE);
    current = p;

    // Check the process's current pagetable.
    check_pagetable(p->pagetable);

    // This function is defined in k-exception.S. It restores the process's
    // registers then jumps back to user mode.
    exception_return(p);

    // should never get here
    while (true) {
    }
}
