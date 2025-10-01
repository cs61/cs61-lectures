###############################################################################
# Exception handlers
#
#   Assembly code defining kernel exception handlers
#   (for interrupts, traps, and faults).

// import constants from kernel.hh and x86-64.h
#include "obj/k-asm.h"

.text

// kernel_entry
//    The bootloader jumps here after loading the kernel.
//    The code initializes `%rsp` to the top of the kernel stack,
//    then jumps to `kernel_start`.
.globl kernel_entry
kernel_entry:
        // initialize stack pointer and base pointer
        movq $KERNEL_STACK_TOP, %rsp
        movq %rsp, %rbp
        // clear `%rflags`
        pushq $0
        popfq
        // check for multiboot command line; if found pass it along
        cmpl $0x2BADB002, %eax
        jne 1f
        testl $4, (%rbx)
        je 1f
        movl 16(%rbx), %edi
        jmp 2f
1:      movq $0, %rdi
2:      // call kernel_start()
        jmp _Z12kernel_startPKc



// Exception handlers and interrupt descriptor table
//    This code creates an exception handler for all 256 possible
//    exceptions, and initializes a table in the
//    `.interrupt_descriptors` section containing those handlers.
//    The `init_hardware` kernel function installs this table.

// The `exception_handler` macro creates one exception handler
.altmacro
.macro exception_handler num
exception_entry_\num:
        // push zero error code, unless exception did so already
    .if \num != INT_DF && (\num < INT_TS || \num > INT_PF) && \num != INT_AC
        pushq $0
    .endif
        // push exception number
        pushq $\num
        // jump to exception entry point
        jmp _Z15exception_entryv

        // add that handler to the `.interrupt_descriptors` section
    .pushsection .interrupt_descriptors, "aw", @progbits
        .quad exception_entry_\num
        .quad 0
    .popsection
.endm

// now create all 256 exception handlers and table
.set exception_number, 0
.rept 256
exception_handler %exception_number
.set exception_number, exception_number + 1
.endr


// Exception entry point
//    Most exception handlers jump here.
.globl _Z15exception_entryv
_Z15exception_entryv:
        push %gs
        push %fs
        pushq %r15
        pushq %r14
        pushq %r13
        pushq %r12
        pushq %r11
        pushq %r10
        pushq %r9
        pushq %r8
        pushq %rdi
        pushq %rsi
        pushq %rbp
        pushq %rbx
        pushq %rdx
        pushq %rcx
        pushq %rax
        movq %rsp, %rdi

        // load kernel page table
        movq $kernel_pagetable, %rax
        movq %rax, %cr3

        call _Z9exceptionP8regstate
        // `exception` should never return.


.globl _Z16exception_returnP4proc
_Z16exception_returnP4proc:
        // check process state
        movl 12(%rdi), %eax
        cmpl $P_RUNNABLE, %eax
        jne proc_runnable_fail

        // load process page table
        movq (%rdi), %rax
        movq %rax, %cr3

        // restore registers
        leaq 16(%rdi), %rsp
        popq %rax
        popq %rcx
        popq %rdx
        popq %rbx
        popq %rbp
        popq %rsi
        popq %rdi
        popq %r8
        popq %r9
        popq %r10
        popq %r11
        popq %r12
        popq %r13
        popq %r14
        popq %r15
        pop %fs
        pop %gs
        addq $16, %rsp

        // return to process
        iretq


// syscall_entry
//    Kernel entry point for the `syscall` instruction

        .globl _Z13syscall_entryv
_Z13syscall_entryv:
        movq %rsp, KERNEL_STACK_TOP - 16 // save entry %rsp to kernel stack
        movq $KERNEL_STACK_TOP, %rsp     // change to kernel stack

        // structure used by `iret`:
        pushq $(SEGSEL_APP_DATA + 3)   // %ss
        subq $8, %rsp                  // skip saved %rsp
        pushq %r11                     // %rflags
        pushq $(SEGSEL_APP_CODE + 3)   // %cs
        pushq %rcx                     // %rip

        // other registers:
        subq $8, %rsp                  // error code unused
        pushq $-1                      // reg_intno
        push %gs
        push %fs
        pushq %r15 // callee saved
        pushq %r14 // callee saved
        pushq %r13 // callee saved
        pushq %r12 // callee saved
        subq $8, %rsp                  // %r11 clobbered by `syscall`
        pushq %r10
        pushq %r9
        pushq %r8
        pushq %rdi
        pushq %rsi
        pushq %rbp // callee saved
        pushq %rbx // callee saved
        pushq %rdx
        subq $8, %rsp                  // %rcx clobbered by `syscall`
        pushq %rax

        // load kernel page table
        movq $kernel_pagetable, %rax
        movq %rax, %cr3

        // call syscall()
        movq %rsp, %rdi
        call _Z7syscallP8regstate

        // check process state
        movq current, %rcx
        movl 12(%rcx), %ecx
        cmpl $P_RUNNABLE, %ecx
        jne proc_runnable_fail

        // load process page table
        movq current, %rcx
        movq (%rcx), %rcx
        movq %rcx, %cr3

        // skip over other registers
        addq $(8 * 19), %rsp

        // return to process
        iretq


proc_runnable_fail:
        xorl %ecx, %ecx
        movq $proc_runnable_assert, %rdx
        xorl %esi, %esi
        movq $k_exception_str, %rdi
        callq _Z11assert_failPKciS0_S0_


.section .rodata.str1.1
k_exception_str:
        .asciz "k-exception.S"
proc_runnable_assert:
        .asciz "current->state == P_RUNNABLE"
