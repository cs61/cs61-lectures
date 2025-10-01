#ifndef WEENSYOS_KERNEL_HH
#define WEENSYOS_KERNEL_HH
#include "x86-64.h"
#include "lib.hh"
#if WEENSYOS_PROCESS
#error "kernel.hh should not be used by process code."
#endif
struct elf_header;
struct elf_program;
struct program_image_segment;
class vmiter;


// kernel.hh
//
//    Functions, constants, and definitions for the kernel.


// Process state constants
#define P_FREE      0                   // free slot
#define P_RUNNABLE  1                   // runnable process
#define P_BLOCKED   2                   // blocked process
#define P_FAULTED   3                   // faulted process

// Process descriptor type
struct proc {
    x86_64_pagetable* pagetable;        // process's page table
    pid_t pid;                          // process ID
    int state;                          // process state (see above)
    regstate regs;                      // process's current registers
    // The first 4 members of `proc` must not change, but you can add more.
};

// Process table
extern proc ptable[MAXNPROC];


// Kernel start address
#define KERNEL_START_ADDR       0x40000
// Top of the kernel stack
#define KERNEL_STACK_TOP        0x80000

// First application-accessible address
#define PROC_START_ADDR         0x100000

// Physical memory size
#define MEMSIZE_PHYSICAL        0x200000
// Number of physical pages
#define NPAGES                  (MEMSIZE_PHYSICAL / PAGESIZE)

// Virtual memory size
#define MEMSIZE_VIRTUAL         0x300000

// physpages
//    Status of physical memory.
//
//    `physpages[I]` is a `physpageinfo` structure corresponding to the `I`th
//    physical page (which contains physical addresses
//    `[I*PAGESIZE,(I+1)*PAGESIZE)`).
//
//    In the handout code, `physpages[I].refcount` represents the number of
//    times physical page `I` is used. Free pages have `refcount == 0`, and
//    (since handout processes never share memory) allocated pages have
//    `refcount == 1`.
//
//    You can add more information to `physpageinfo` if you need to.
//    The memory viewer calls `used()` and `valid()` to check for bugs.
struct physpageinfo {
    uint8_t refcount = 0;

    bool used() const {
        return this->refcount != 0;
    }
    bool valid() const {
        return this->refcount <= MAXNPROC;
    }
};
extern physpageinfo physpages[NPAGES];


// Segment selectors
#define SEGSEL_BOOT_CODE        0x8             // boot code segment
#define SEGSEL_KERN_CODE        0x8             // kernel code segment
#define SEGSEL_KERN_DATA        0x10            // kernel data segment
#define SEGSEL_APP_CODE         0x18            // application code segment
#define SEGSEL_APP_DATA         0x20            // application data segment
#define SEGSEL_TASKSTATE        0x28            // task state segment


// Hardware interrupt numbers
#define INT_IRQ                 32U
#define IRQ_TIMER               0
#define IRQ_KEYBOARD            1
#define IRQ_ERROR               19
#define IRQ_SPURIOUS            31


// init_hardware
//    Initialize x86 hardware, including memory, interrupts, and segments.
//    All accessible physical memory is initially mapped as readable
//    and writable to both kernel and application code.
void init_hardware();

// init_timer(rate)
//    Set the timer interrupt to fire `rate` times a second. Disables the
//    timer interrupt if `rate <= 0`.
void init_timer(int rate);


void* kalloc(size_t sz);
void kfree(void* ptr);


// kernel page table (used for virtual memory)
extern x86_64_pagetable kernel_pagetable[];

// reserved_physical_address(pa)
//    Returns non-zero iff `pa` is a reserved physical address.
bool reserved_physical_address(uintptr_t pa);

// allocatable_physical_address(pa)
//    Returns non-zero iff `pa` is an allocatable physical address.
bool allocatable_physical_address(uintptr_t pa);

// kalloc_pagetable
//    Allocate and initialize a new, empty level-4 page table.
x86_64_pagetable* kalloc_pagetable();

// check_process_registers
//    Validate a process by checking its registers for common errors.
void check_process_registers(const proc* p);

// check_pagetable
//    Validate a page table by checking that important kernel procedures
//    are mapped at the expected addresses.
void check_pagetable(x86_64_pagetable* pagetable);

// set_pagetable
//    Change page table after checking it.
void set_pagetable(x86_64_pagetable* pagetable);

// check_page_table_mappings
//    Check operating system invariants about kernel mappings for a page
//    table. Panic if any of the invariants are false.
void check_page_table_mappings(x86_64_pagetable* pagetable);

// exception_entry
//    Entry point for exceptions (interrupts, traps, and faults). Defined
//    in `k-exception.S`; “called” only by hardware.
void exception_entry();

// syscall_entry
//    Entry point for system calls (the `syscall` instruction). Defined in
//    `k-exception.S`; “called” only by hardware.
void syscall_entry();

// exception_return
//    Return from an exception to user mode: load the page table
//    and registers and start the process back up. Defined in k-exception.S.
[[noreturn]] void exception_return(proc* p);


// console_show_cursor()
//    Show the console cursor at `cursorpos`, which should be between 0
//    and 80 * 25.
void console_show_cursor();


// keyboard_readc
//    Read a character from the keyboard. Returns -1 if there is no character
//    to read, and 0 if no real key press was registered but you should call
//    keyboard_readc() again (e.g. the user pressed a SHIFT key). Otherwise
//    returns either an ASCII character code or one of the special characters
//    listed below.
int keyboard_readc();

#define KEY_UP          0300
#define KEY_RIGHT       0301
#define KEY_DOWN        0302
#define KEY_LEFT        0303
#define KEY_HOME        0304
#define KEY_END         0305
#define KEY_PAGEUP      0306
#define KEY_PAGEDOWN    0307
#define KEY_INSERT      0310
#define KEY_DELETE      0311

// check_keyboard
//    Check for the user typing a control key. 'a', 'f', and 'e' cause a soft
//    reboot where the kernel runs the allocator programs, "fork", or
//    "forkexit", respectively. Control-C or 'q' exit the virtual machine.
//    Returns key typed or -1 for no key.
int check_keyboard();


// init_process(p, flags)
//    Initialize special-purpose registers for process `p`. Constants for
//    `flags` are listed below.
void init_process(proc* p, int flags);

#define PROCINIT_ALLOW_PROGRAMMED_IO    0x01
#define PROCINIT_DISABLE_INTERRUPTS     0x02


// program_image
//    Represents a program image. Use it to iterate over the loadable
//    memory segments of a program.

struct program_image {
    // Load a program image by number or name.
    program_image(int program_number);
    program_image(const char* program_name);
    static int program_number(const char* program_name);

    // Return true iff this program image is empty (has no data).
    bool empty() const;

    // Return an iterator to the beginning loadable segment in the image.
    program_image_segment begin() const;
    // Return an iterator to the end loadable segment in the image.
    program_image_segment end() const;

    // Return the user virtual address of the entry point instruction.
    uintptr_t entry() const;

  private:
    elf_header* elf_;
};

struct program_image_segment {
    // Return the user virtual address where the segment should be loaded.
    uintptr_t va() const;
    // Return the size of the segment, including zero-initialized space.
    size_t size() const;
    // Return a pointer to the first byte of data in this segment.
    const char* data() const;
    // Return the number of data bytes that should be copied from `data()`.
    // Always `data_size() <= size()`; bytes between `data_size()` and
    // `size()` should be zero-initialized.
    size_t data_size() const;

    // Return true iff the segment is writable.
    bool writable() const;

    // Compare segment iterators.
    bool operator==(const program_image_segment& x) const;
    bool operator!=(const program_image_segment& x) const;

    // Advance to the next segment.
    void operator++();
    void operator++(int);             // move to next segment

  private:
    elf_program* ph_;
    elf_header* elf_;

    program_image_segment(elf_program* ph, elf_header* elf);
    inline void fix();
    friend struct program_image;
};


// poweroff
//    Turn off the virtual machine.
[[noreturn]] void poweroff();

// reboot
//    Reboot the virtual machine.
[[noreturn]] void reboot();

// proc_panic(p, fmt, ...)
//    Report a panic that happened due to a process & its registers.
[[noreturn]] void proc_panic(const proc* p, const char* fmt, ...);

// user_panic(p)
//    Report a panic caused by a PROC_PANIC system call.
[[noreturn]] void user_panic(const proc* p);

// error_print_backtrace(regs, pagetable)
//    Print a backtrace starting from `regs` in `pagetable`.
void error_print_backtrace(const regstate& regs, x86_64_pagetable* pagetable,
                           bool exclude_rip = false);


// strlcpy_from_user
//    Copy a C string from a `vmiter` into `buf`, only considering
//    user-accessible mappings.
void strlcpy_from_user(char* buf, vmiter it, size_t maxlen);


// log_printf, log_vprintf
//    Print debugging messages to the host's `log.txt` file. We run QEMU
//    so that messages written to the QEMU "parallel port" end up in `log.txt`.
__noinline void log_printf(const char* format, ...);
__noinline void log_vprintf(const char* format, va_list val);

// log_print_backtrace
//    Print a backtrace to the host's `log.txt` file, either for the current
//    stack or for the stack active in `p`.
void log_print_backtrace();
void log_print_backtrace(const proc* p);

// lookup_symbol
//    Read the hidden symbol table for the name of the kernel symbol at
//    `addr`, storing the name in `*name` and the starting address in `*start`.
//    Returns true if found.
__no_asan
bool lookup_symbol(uintptr_t addr, const char** name, uintptr_t* start);


// error_vprintf, error_printf
//    Print debugging messages to the console and to the host's
//    `log.txt` file via `log_printf`.
void error_printf(const char* format, ...);


// address translation functions for identity-mapped kernels
inline uint64_t kptr2pa(uint64_t kptr) {
    return kptr;
}
template <typename T> inline uint64_t kptr2pa(T* kptr) {
    return reinterpret_cast<uint64_t>(kptr);
}
inline uint64_t pa2ka(uint64_t pa) {
    return pa;
}
template <typename T = void*> inline T pa2kptr(uint64_t pa) {
    static_assert(std::is_pointer<T>::value, "T must be pointer");
    return reinterpret_cast<T>(pa);
}

#endif
