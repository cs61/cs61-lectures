#ifndef WEENSYOS_U_LIB_HH
#define WEENSYOS_U_LIB_HH
#include "lib.hh"
#include "x86-64.h"
#if WEENSYOS_KERNEL
#error "u-lib.hh should not be used by kernel code."
#endif

// u-lib.hh
//
//    Support code for WeensyOS user-level code.


// make_syscall
//    These functions define the WeensyOS system call calling convention.
//    We provide versions for system calls with 0-2 arguments.

__always_inline
uintptr_t make_syscall(int syscallno) {
    register uintptr_t rax asm("rax") = syscallno;
    asm volatile ("syscall"
            : "+a" (rax)
            : /* all input registers are also output registers */
            : "cc", "memory", "rcx", "rdx", "rsi", "rdi", "r8", "r9",
              "r10", "r11");
    return rax;
}

__always_inline
uintptr_t make_syscall(int syscallno, uintptr_t arg0) {
    register uintptr_t rax asm("rax") = syscallno;
    asm volatile ("syscall"
            : "+a" (rax), "+D" (arg0)
            :
            : "cc", "memory", "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11");
    return rax;
}

__always_inline
uintptr_t make_syscall(int syscallno, uintptr_t arg0, uintptr_t arg1) {
    register uintptr_t rax asm("rax") = syscallno;
    asm volatile ("syscall"
            : "+a" (rax), "+D" (arg0), "+S" (arg1)
            :
            : "cc", "memory", "rcx", "rdx", "r8", "r9", "r10", "r11");
    return rax;
}

__always_inline
uintptr_t make_syscall(int syscallno, uintptr_t arg0,
                       uintptr_t arg1, uintptr_t arg2) {
    register uintptr_t rax asm("rax") = syscallno;
    asm volatile ("syscall"
            : "+a" (rax), "+D" (arg0), "+S" (arg1), "+d" (arg2)
            :
            : "cc", "rcx", "r8", "r9", "r10", "r11");
    return rax;
}

__always_inline
uintptr_t make_syscall(int syscallno, uintptr_t arg0,
                       uintptr_t arg1, uintptr_t arg2, uintptr_t arg3) {
    register uintptr_t rax asm("rax") = syscallno;
    register uintptr_t r10 asm("r10") = arg3;
    asm volatile ("syscall"
            : "+a" (rax), "+D" (arg0), "+S" (arg1), "+d" (arg2), "+r" (r10)
            :
            : "cc", "rcx", "r8", "r9", "r11");
    return rax;
}

__always_inline
void clobber_memory(void* ptr) {
    asm volatile ("" : "+m" (*(char*) ptr));
}

__always_inline
void access_memory(const void* ptr) {
    asm volatile ("" : : "m" (*(const char*) ptr));
}


// System call declarations
// Definitions and explanations of system calls are in `u-lib.cc`.

pid_t sys_getpid();
int sys_yield();

int sys_getsysname(char* buf);
ssize_t sys_write(const char* msg, size_t n);
int sys_page_alloc(void* addr);

[[noreturn]] void sys_panic(const char* msg);

#endif
