#include "kernel.hh"
#include "lib.hh"
#include "elf.h"
#include "k-apic.hh"
#include "k-pci.hh"
#include "k-vmiter.hh"
#include "obj/k-foreachimage.h"
#include <atomic>


// k-hardware.cc
//
//    Functions for interacting with x86 hardware.


// init_hardware
//    Initialize hardware. Calls other functions below.

pcistate pcistate::state;

static void init_kernel_memory();
static void init_interrupts();
static void init_constructors();
static void init_cpu_hardware();
static void stash_kernel_data(bool restore);

void init_hardware() {
    // initialize kernel virtual memory structures
    init_kernel_memory();

    // initialize console position
    cursorpos = 3 * CONSOLE_COLUMNS;

    // initialize interrupt descriptors and controller
    init_interrupts();

    // call C++ constructors for global objects
    // (NB none of these constructors may allocate memory)
    init_constructors();

    // initialize this CPU
    init_cpu_hardware();
}


// init_kernel_memory
//    Set up early-stage segment registers and kernel page table.
//
//    The early-stage segment registers and global descriptors are
//    used during hardware initialization. The kernel page table is
//    used whenever no appropriate process page table exists.
//
//    The interrupt descriptor table tells the processor where to jump
//    when an interrupt or exception happens. See k-exception.S.
//
//    The layouts of these types are defined by the hardware.

static void set_app_segment(uint64_t* segment, uint64_t type, int dpl) {
    *segment = type
        | X86SEG_S                    // code/data segment
        | (uint64_t(dpl) << 45)
        | X86SEG_P;                   // segment present
}

static void set_sys_segment(uint64_t* segment, uintptr_t addr, size_t size,
                            uint64_t type, int dpl) {
    segment[0] = ((addr & 0x0000000000FFFFFFUL) << 16)
        | ((addr & 0x00000000FF000000UL) << 32)
        | ((size - 1) & 0x0FFFFUL)
        | (((size - 1) & 0xF0000UL) << 48)
        | type
        | (uint64_t(dpl) << 45)
        | X86SEG_P;                   // segment present
    segment[1] = addr >> 32;
}

static void set_gate(x86_64_gatedescriptor* gate, uintptr_t addr,
                     int type, int dpl, int ist) {
    assert(unsigned(type) < 16 && unsigned(dpl) < 4 && unsigned(ist) < 8);
    gate->gd_low = (addr & 0x000000000000FFFFUL)
        | (SEGSEL_KERN_CODE << 16)
        | (uint64_t(ist) << 32)
        | (uint64_t(type) << 40)
        | (uint64_t(dpl) << 45)
        | X86SEG_P
        | ((addr & 0x00000000FFFF0000UL) << 32);
    gate->gd_high = addr >> 32;
}

x86_64_pagetable kernel_pagetable[5];
static uint64_t gdt_segments[7];

void init_kernel_memory() {
    stash_kernel_data(false);

    // initialize segment descriptors for kernel code and data
    gdt_segments[0] = 0;
    set_app_segment(&gdt_segments[SEGSEL_KERN_CODE >> 3],
                    X86SEG_X | X86SEG_L, 0);
    set_app_segment(&gdt_segments[SEGSEL_KERN_DATA >> 3],
                    X86SEG_W, 0);
    x86_64_pseudodescriptor gdt;
    gdt.limit = sizeof(gdt_segments[0]) * 3 - 1;
    gdt.base = reinterpret_cast<uint64_t>(gdt_segments);

    asm volatile("lgdt %0" : : "m" (gdt.limit));


    // initialize kernel page table
    memset(kernel_pagetable, 0, sizeof(kernel_pagetable));
    kernel_pagetable[0].entry[0] =
        kptr2pa(&kernel_pagetable[1]) | PTE_P | PTE_W | PTE_U;
    kernel_pagetable[1].entry[0] =
        kptr2pa(&kernel_pagetable[2]) | PTE_P | PTE_W | PTE_U;
    kernel_pagetable[2].entry[0] =
        kptr2pa(&kernel_pagetable[3]) | PTE_P | PTE_W | PTE_U;
    kernel_pagetable[2].entry[1] =
        kptr2pa(&kernel_pagetable[4]) | PTE_P | PTE_W | PTE_U;

    // the kernel can access [1GiB,4GiB) of physical memory,
    // which includes important memory-mapped I/O devices
    kernel_pagetable[1].entry[1] =
        (1UL << 30) | PTE_P | PTE_W | PTE_PS;
    kernel_pagetable[1].entry[2] =
        (2UL << 30) | PTE_P | PTE_W | PTE_PS;
    kernel_pagetable[1].entry[3] =
        (3UL << 30) | PTE_P | PTE_W | PTE_PS;

    // user-accessible mappings for physical memory,
    // except that (for debuggability) nullptr is totally inaccessible
    for (vmiter it(kernel_pagetable, 0);
         it.va() < MEMSIZE_PHYSICAL;
         it += PAGESIZE) {
        if (it.va() != 0) {
            it.map(it.va(), PTE_P | PTE_W | PTE_U);
        }
    }

    wrcr3(kptr2pa(kernel_pagetable));


    // Now that boot-time structures (pagetable and global descriptor
    // table) have been replaced, we can reuse boot-time memory.
}


// init_interrupts

// processor state for taking an interrupt
extern x86_64_gatedescriptor interrupt_descriptors[256];

void init_interrupts() {
    // initialize interrupt descriptors
    // Macros in `k-exception.S` initialized `interrupt_descriptors[]`
    // with function pointers in the `gd_low` members. We must change
    // them to the weird format x86-64 expects.
    for (int i = 0; i < 256; ++i) {
        uintptr_t addr = interrupt_descriptors[i].gd_low;
        set_gate(&interrupt_descriptors[i], addr,
                 X86GATE_INTERRUPT, i == INT_BP ? 3 : 0, 0);
    }

    // ensure machine has an enabled APIC
    assert(cpuid(1).edx & (1 << 9));
    uint64_t apic_base = rdmsr(MSR_IA32_APIC_BASE);
    assert(apic_base & IA32_APIC_BASE_ENABLED);
    assert((apic_base & 0xFFFFFFFFF000) == lapicstate::lapic_pa);

    // ensure machine has an IOAPIC
    auto& ioapic = ioapicstate::get();
    uint32_t ioapic_ver = ioapic.read(ioapic.reg_ver);
    assert((ioapic_ver & 0xFF) == 0x11 || (ioapic_ver & 0xFF) == 0x20);
    assert((ioapic_ver >> 16) >= 0x17);

    // disable the old programmable interrupt controller
#define IO_PIC1         0x20    // Master (IRQs 0-7)
#define IO_PIC2         0xA0    // Slave (IRQs 8-15)
    outb(IO_PIC1 + 1, 0xFF);
    outb(IO_PIC2 + 1, 0xFF);
}


void init_constructors() {
    typedef void (*constructor_function)();
    extern constructor_function __init_array_start[];
    extern constructor_function __init_array_end[];
    for (auto fp = __init_array_start; fp != __init_array_end; ++fp) {
        (*fp)();
    }
}


static x86_64_taskstate taskstate;

void init_cpu_hardware() {
    // initialize per-CPU segments
    gdt_segments[0] = 0;
    set_app_segment(&gdt_segments[SEGSEL_KERN_CODE >> 3],
                    X86SEG_X | X86SEG_L, 0);
    set_app_segment(&gdt_segments[SEGSEL_KERN_DATA >> 3],
                    X86SEG_W, 0);
    set_app_segment(&gdt_segments[SEGSEL_APP_CODE >> 3],
                    X86SEG_X | X86SEG_L, 3);
    set_app_segment(&gdt_segments[SEGSEL_APP_DATA >> 3],
                    X86SEG_W, 3);
    set_sys_segment(&gdt_segments[SEGSEL_TASKSTATE >> 3],
                    reinterpret_cast<uintptr_t>(&taskstate), sizeof(taskstate),
                    X86SEG_TSS, 0);

    // taskstate lets the kernel receive interrupts
    memset(&taskstate, 0, sizeof(taskstate));
    taskstate.ts_rsp[0] = KERNEL_STACK_TOP;

    x86_64_pseudodescriptor gdt, idt;
    gdt.limit = sizeof(gdt_segments) - 1;
    gdt.base = reinterpret_cast<uint64_t>(gdt_segments);
    idt.limit = sizeof(interrupt_descriptors) - 1;
    idt.base = reinterpret_cast<uint64_t>(interrupt_descriptors);

    // load segment descriptor tables
    asm volatile("lgdt %0; ltr %1; lidt %2"
                 :
                 : "m" (gdt.limit),
                   "r" (uint16_t(SEGSEL_TASKSTATE)),
                   "m" (idt.limit)
                 : "memory", "cc");

    // initialize segments
    asm volatile("movw %%ax, %%fs; movw %%ax, %%gs"
                 : : "a" (uint16_t(SEGSEL_KERN_DATA)));


    // set up control registers
    uint32_t cr0 = rdcr0();
    cr0 |= CR0_PE | CR0_PG | CR0_WP | CR0_AM | CR0_MP | CR0_NE;
    wrcr0(cr0);


    // set up syscall/sysret
    wrmsr(MSR_IA32_STAR, (uintptr_t(SEGSEL_KERN_CODE) << 32)
          | (uintptr_t(SEGSEL_APP_CODE) << 48));
    wrmsr(MSR_IA32_LSTAR, reinterpret_cast<uint64_t>(syscall_entry));
    wrmsr(MSR_IA32_FMASK, EFLAGS_TF | EFLAGS_DF | EFLAGS_IF
          | EFLAGS_IOPL_MASK | EFLAGS_AC | EFLAGS_NT);


    // initialize local APIC (interrupt controller)
    auto& lapic = lapicstate::get();
    lapic.enable_lapic(INT_IRQ + IRQ_SPURIOUS);

    // timer is in periodic mode
    lapic.write(lapic.reg_timer_divide, lapic.timer_divide_1);
    lapic.write(lapic.reg_lvt_timer,
                lapic.timer_periodic | (INT_IRQ + IRQ_TIMER));
    lapic.write(lapic.reg_timer_initial_count, 0);

    // disable logical interrupt lines
    lapic.write(lapic.reg_lvt_lint0, lapic.lvt_masked);
    lapic.write(lapic.reg_lvt_lint1, lapic.lvt_masked);

    // set LVT error handling entry
    lapic.write(lapic.reg_lvt_error, INT_IRQ + IRQ_ERROR);

    // clear error status by reading the error;
    // acknowledge any outstanding interrupts
    lapic.error();
    lapic.ack();
}


// init_timer(rate)
//    Set the timer interrupt to fire `rate` times a second. Disables the
//    timer interrupt if `rate <= 0`.

void init_timer(int rate) {
    auto& lapic = lapicstate::get();
    if (rate > 0) {
        lapic.write(lapic.reg_timer_initial_count, 1000000000 / rate);
    } else {
        lapic.write(lapic.reg_timer_initial_count, 0);
    }
}


// kalloc(sz)
//    Kernel memory allocator. Allocates `sz` contiguous bytes and
//    returns a pointer to the allocated memory, or `nullptr` on failure.
//
//    On WeensyOS, `kalloc` is a page-based allocator: if `sz > PAGESIZE`
//    the allocation fails; if `sz < PAGESIZE` it allocates a whole page
//    anyway.

void* kalloc(size_t sz) {
    assert(sz <= PAGESIZE);
    for (uintptr_t pa = PAGESIZE; pa < MEMSIZE_PHYSICAL; pa += PAGESIZE) {
        if (allocatable_physical_address(pa)
            && physpages[pa / PAGESIZE].refcount == 0) {
            ++physpages[pa / PAGESIZE].refcount;
            memset((void*) pa, 0xCC, PAGESIZE);
            return (void*) pa;
        }
    }
    return nullptr;
}


// kalloc_pagetable
//    Allocate and return a new, empty page table.

x86_64_pagetable* kalloc_pagetable() {
    x86_64_pagetable* pt = reinterpret_cast<x86_64_pagetable*>(kalloc(PAGESIZE));
    if (pt) {
        memset(&pt->entry[0], 0, sizeof(x86_64_pagetable));
    }
    return pt;
}


// check_process_registers
//    Validate a process’s registers by checking for common errors.

void check_process_registers(const proc* p) {
    assert(p->regs.reg_rsp >= PROC_START_ADDR);
    assert(p->regs.reg_rsp <= MEMSIZE_VIRTUAL);
    assert((p->regs.reg_cs & 3) == 3);
}


// check_pagetable
//    Validate a page table by checking that important kernel procedures
//    are mapped at the expected addresses.

void check_pagetable(x86_64_pagetable* pagetable) {
    // `pagetable` is page-aligned
    assert((reinterpret_cast<uintptr_t>(pagetable) % PAGESIZE) == 0);
    // `pagetable` contains some critical mappings
    assert(vmiter(pagetable, reinterpret_cast<uintptr_t>(exception_entry)).pa()
           == kptr2pa(exception_entry));
    assert(vmiter(kernel_pagetable, reinterpret_cast<uintptr_t>(pagetable)).pa()
           == kptr2pa(pagetable));
    assert(vmiter(pagetable, reinterpret_cast<uintptr_t>(kernel_pagetable)).pa()
           == kptr2pa(kernel_pagetable));
}


// set_pagetable
//    Change page table after checking it.

void set_pagetable(x86_64_pagetable* pagetable) {
    check_pagetable(pagetable);
    wrcr3(kptr2pa(pagetable));
}


// reserved_physical_address(pa)
//    Returns true iff `pa` is a reserved physical address.

#define IOPHYSMEM       0x000A0000
#define EXTPHYSMEM      0x00100000

bool reserved_physical_address(uintptr_t pa) {
    return pa < PAGESIZE || (pa >= IOPHYSMEM && pa < EXTPHYSMEM);
}


// allocatable_physical_address(pa)
//    Returns true iff `pa` is an allocatable physical address, i.e.,
//    not reserved or holding kernel data.

bool allocatable_physical_address(uintptr_t pa) {
    extern uint8_t _kernel_end[];
    return !reserved_physical_address(pa)
        && (pa < KERNEL_START_ADDR
            || pa >= round_up(reinterpret_cast<uintptr_t>(_kernel_end), PAGESIZE))
        && (pa < KERNEL_STACK_TOP - PAGESIZE
            || pa >= KERNEL_STACK_TOP)
        && pa < MEMSIZE_PHYSICAL;
}


// pcistate::next(addr)
//    Return the next valid PCI function after `addr`, or -1 if there
//    is none.
int pcistate::next(int addr) const {
    uint32_t x = readl(addr + config_lthb);
    while (true) {
        if (addr_func(addr) == 0
            && (x == uint32_t(-1) || !(x & 0x800000))) {
            addr += make_addr(0, 1, 0);
        } else {
            addr += make_addr(0, 0, 1);
        }
        if (addr >= addr_end) {
            return -1;
        }
        x = readl(addr + config_lthb);
        if (x != uint32_t(-1)) {
            return addr;
        }
    }
}

void pcistate::enable(int addr) {
    // enable I/O (0x01), memory (0x02), and bus master (0x04)
    writew(addr + config_command, 0x0007);
}


// poweroff
//    Turn off the virtual machine. This requires finding a PCI device
//    that speaks ACPI.

void poweroff() {
    auto& pci = pcistate::get();
    int addr = pci.find([&] (int a) {
            uint32_t vd = pci.readl(a + pci.config_vendor);
            return vd == 0x71138086U /* PIIX4 Power Management Controller */
                || vd == 0x29188086U /* ICH9 LPC Interface Controller */;
        });
    if (addr >= 0) {
        // Read I/O base register from controller's PCI configuration space.
        int pm_io_base = pci.readl(addr + 0x40) & 0xFFC0;
        // Write `suspend enable` to the power management control register.
        outw(pm_io_base + 4, 0x2000);
    }
    // No known ACPI controller; spin.
    console_printf(CPOS(24, 0), CS_ERROR "Cannot power off!\n");
    while (true) {
    }
}


// reboot
//    Reboot the virtual machine.

void reboot() {
    outb(0x92, 3); // does not return
    while (true) {
    }
}


// init_process(p, flags)
//    Initialize special-purpose registers for process `p`.

void init_process(proc* p, int flags) {
    memset(&p->regs, 0, sizeof(p->regs));
    p->regs.reg_cs = SEGSEL_APP_CODE | 3;
    p->regs.reg_fs = SEGSEL_APP_DATA | 3;
    p->regs.reg_gs = SEGSEL_APP_DATA | 3;
    p->regs.reg_ss = SEGSEL_APP_DATA | 3;
    p->regs.reg_rflags = EFLAGS_IF;

    if (flags & PROCINIT_ALLOW_PROGRAMMED_IO) {
        p->regs.reg_rflags |= EFLAGS_IOPL_3;
    }
    if (flags & PROCINIT_DISABLE_INTERRUPTS) {
        p->regs.reg_rflags &= ~EFLAGS_IF;
    }
}


// console_show_cursor()
//    Show the console cursor to position `cursorpos`, which should be
//    between 0 and 80 * 25.

void console_show_cursor() {
    if (cursorpos < 0 || cursorpos > CONSOLE_ROWS * CONSOLE_COLUMNS) {
        cursorpos = 0;
    }
    outb(0x3D4, 14);
    outb(0x3D5, cursorpos / 256);
    outb(0x3D4, 15);
    outb(0x3D5, cursorpos % 256);
}



// keyboard_readc
//    Read a character from the keyboard. Returns -1 if there is no character
//    to read, and 0 if no real key press was registered but you should call
//    keyboard_readc() again (e.g. the user pressed a SHIFT key). Otherwise
//    returns either an ASCII character code or one of the special characters
//    listed in kernel.hh.

// Unfortunately mapping PC key codes to ASCII takes a lot of work.

#define MOD_SHIFT       (1 << 0)
#define MOD_CONTROL     (1 << 1)
#define MOD_CAPSLOCK    (1 << 3)

#define KEY_SHIFT       0xFA
#define KEY_CONTROL     0xFB
#define KEY_ALT         0xFC
#define KEY_CAPSLOCK    0xFD
#define KEY_NUMLOCK     0xFE
#define KEY_SCROLLLOCK  0xFF

#define CKEY(cn)        (0x80 + cn)

static const uint8_t keymap[256] = {
    /*0x00*/ 0, 033, CKEY(0), CKEY(1), CKEY(2), CKEY(3), CKEY(4), CKEY(5),
        CKEY(6), CKEY(7), CKEY(8), CKEY(9), CKEY(10), CKEY(11), '\b', '\t',
    /*0x10*/ 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
        'o', 'p', CKEY(12), CKEY(13), CKEY(14), KEY_CONTROL, 'a', 's',
    /*0x20*/ 'd', 'f', 'g', 'h', 'j', 'k', 'l', CKEY(15),
        CKEY(16), CKEY(17), KEY_SHIFT, CKEY(18), 'z', 'x', 'c', 'v',
    /*0x30*/ 'b', 'n', 'm', CKEY(19), CKEY(20), CKEY(21), KEY_SHIFT, '*',
        KEY_ALT, ' ', KEY_CAPSLOCK, 0, 0, 0, 0, 0,
    /*0x40*/ 0, 0, 0, 0, 0, KEY_NUMLOCK, KEY_SCROLLLOCK, '7',
        '8', '9', '-', '4', '5', '6', '+', '1',
    /*0x50*/ '2', '3', '0', '.', 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0x60*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0x70*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0x80*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0x90*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, CKEY(14), KEY_CONTROL, 0, 0,
    /*0xA0*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0xB0*/ 0, 0, 0, 0, 0, '/', 0, 0,  KEY_ALT, 0, 0, 0, 0, 0, 0, 0,
    /*0xC0*/ 0, 0, 0, 0, 0, 0, 0, KEY_HOME,
        KEY_UP, KEY_PAGEUP, 0, KEY_LEFT, 0, KEY_RIGHT, 0, KEY_END,
    /*0xD0*/ KEY_DOWN, KEY_PAGEDOWN, KEY_INSERT, KEY_DELETE, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    /*0xE0*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0xF0*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0
};

static const struct keyboard_key {
    uint8_t map[4];
} complex_keymap[] = {
    /*CKEY(0)*/ {{'1', '!', 0, 0}},      /*CKEY(1)*/ {{'2', '@', 0, 0}},
    /*CKEY(2)*/ {{'3', '#', 0, 0}},      /*CKEY(3)*/ {{'4', '$', 0, 0}},
    /*CKEY(4)*/ {{'5', '%', 0, 0}},      /*CKEY(5)*/ {{'6', '^', 0, 0x1E}},
    /*CKEY(6)*/ {{'7', '&', 0, 0}},      /*CKEY(7)*/ {{'8', '*', 0, 0}},
    /*CKEY(8)*/ {{'9', '(', 0, 0}},      /*CKEY(9)*/ {{'0', ')', 0, 0}},
    /*CKEY(10)*/ {{'-', '_', 0, 0x1F}},  /*CKEY(11)*/ {{'=', '+', 0, 0}},
    /*CKEY(12)*/ {{'[', '{', 0x1B, 0}},  /*CKEY(13)*/ {{']', '}', 0x1D, 0}},
    /*CKEY(14)*/ {{'\n', '\n', '\r', '\r'}},
    /*CKEY(15)*/ {{';', ':', 0, 0}},
    /*CKEY(16)*/ {{'\'', '"', 0, 0}},    /*CKEY(17)*/ {{'`', '~', 0, 0}},
    /*CKEY(18)*/ {{'\\', '|', 0x1C, 0}}, /*CKEY(19)*/ {{',', '<', 0, 0}},
    /*CKEY(20)*/ {{'.', '>', 0, 0}},     /*CKEY(21)*/ {{'/', '?', 0, 0}}
};

int keyboard_readc() {
    static uint8_t modifiers;
    static uint8_t last_escape;

    if ((inb(KEYBOARD_STATUSREG) & KEYBOARD_STATUS_READY) == 0) {
        return -1;
    }

    uint8_t data = inb(KEYBOARD_DATAREG);
    uint8_t escape = last_escape;
    last_escape = 0;

    if (data == 0xE0) {         // mode shift
        last_escape = 0x80;
        return 0;
    } else if (data & 0x80) {   // key release: matters only for modifier keys
        int ch = keymap[(data & 0x7F) | escape];
        if (ch >= KEY_SHIFT && ch < KEY_CAPSLOCK) {
            modifiers &= ~(1 << (ch - KEY_SHIFT));
        }
        return 0;
    }

    int ch = (unsigned char) keymap[data | escape];

    if (ch >= 'a' && ch <= 'z') {
        if (modifiers & MOD_CONTROL) {
            ch -= 0x60;
        } else if (!(modifiers & MOD_SHIFT) != !(modifiers & MOD_CAPSLOCK)) {
            ch -= 0x20;
        }
    } else if (ch >= KEY_CAPSLOCK) {
        modifiers ^= 1 << (ch - KEY_SHIFT);
        ch = 0;
    } else if (ch >= KEY_SHIFT) {
        modifiers |= 1 << (ch - KEY_SHIFT);
        ch = 0;
    } else if (ch >= CKEY(0) && ch <= CKEY(21)) {
        ch = complex_keymap[ch - CKEY(0)].map[modifiers & 3];
    } else if (ch < 0x80 && (modifiers & MOD_CONTROL)) {
        ch = 0;
    }

    return ch;
}


// log_printf, log_vprintf
//    Print debugging messages to the host's `log.txt` file. We run QEMU
//    so that messages written to the QEMU "parallel port" end up in `log.txt`.

#define IO_PARALLEL1_DATA       0x378
#define IO_PARALLEL1_STATUS     0x379
# define IO_PARALLEL_STATUS_BUSY        0x80
#define IO_PARALLEL1_CONTROL    0x37A
# define IO_PARALLEL_CONTROL_SELECT     0x08
# define IO_PARALLEL_CONTROL_INIT       0x04
# define IO_PARALLEL_CONTROL_STROBE     0x01

static void delay() {
    (void) inb(0x84);
    (void) inb(0x84);
    (void) inb(0x84);
    (void) inb(0x84);
}

static void parallel_port_putc(unsigned char c) {
    static int initialized;
    if (!initialized) {
        outb(IO_PARALLEL1_CONTROL, 0);
        initialized = 1;
    }

    for (int i = 0;
         i < 12800 && (inb(IO_PARALLEL1_STATUS) & IO_PARALLEL_STATUS_BUSY) == 0;
         ++i) {
        delay();
    }
    outb(IO_PARALLEL1_DATA, c);
    outb(IO_PARALLEL1_CONTROL, IO_PARALLEL_CONTROL_SELECT
         | IO_PARALLEL_CONTROL_INIT | IO_PARALLEL_CONTROL_STROBE);
    outb(IO_PARALLEL1_CONTROL, IO_PARALLEL_CONTROL_SELECT
         | IO_PARALLEL_CONTROL_INIT);
}

namespace {
struct log_printer : public printer {
    ansi_escape_buffer ebuf_;
    void putc(unsigned char c) override {
        if (!ebuf_.putc(c, *this)) {
            parallel_port_putc(c);
        }
    }
};

struct error_printer : public console_printer {
    log_printer logpr_;
    error_printer()
        : console_printer(-1, scroll_blank) {
        if (cell_ < console + END_CPOS - CONSOLE_COLUMNS) {
            cell_ = console + END_CPOS;
        }
        color_ = COLOR_ERROR;
    }
    void putc(unsigned char c) override {
        logpr_.putc(c);
        console_printer::putc(c);
    }
};
}

void log_vprintf(const char* format, va_list val) {
    log_printer pr;
    pr.vprintf(format, val);
}

void log_printf(const char* format, ...) {
    va_list val;
    va_start(val, format);
    log_vprintf(format, val);
    va_end(val);
}


// symtab: reference to kernel symbol table; useful for debugging.
// The `mkchickadeesymtab` program fills this structure in.
#define SYMTAB_ADDR 0x1000000
elf_symtabref symtab = {
    reinterpret_cast<elf_symbol*>(SYMTAB_ADDR), 0, nullptr, 0
};

// lookup_symbol(addr, name, start)
//    Use the debugging symbol table to look up `addr`. Return the
//    corresponding symbol name (usually a function name) in `*name`
//    and the first address in that symbol in `*start`.

__no_asan
bool lookup_symbol(uintptr_t addr, const char** name, uintptr_t* start) {
    extern elf_symtabref symtab;
    if (!kernel_pagetable[2].entry[SYMTAB_ADDR / 0x200000]) {
        kernel_pagetable[2].entry[SYMTAB_ADDR / 0x200000] =
            SYMTAB_ADDR | PTE_P | PTE_W | PTE_PS;
    }

    size_t l = 0;
    size_t r = symtab.nsym;
    while (l < r) {
        size_t m = l + ((r - l) >> 1);
        auto& sym = symtab.sym[m];
        if (sym.st_value <= addr
            && (m + 1 == symtab.nsym
                ? addr < sym.st_value + 0x1000
                : addr < (&sym)[1].st_value)
            && (sym.st_size == 0 || addr <= sym.st_value + sym.st_size)) {
            if (!sym.st_value) {
                return false;
            }
            if (name) {
                *name = symtab.strtab + sym.st_name;
            }
            if (start) {
                *start = sym.st_value;
            }
            return true;
        } else if (sym.st_value < addr) {
            l = m + 1;
        } else {
            r = m;
        }
    }
    return false;
}


namespace {
struct backtrace_regs {
    uintptr_t reg_rip;
    uintptr_t reg_rsp;
    uintptr_t reg_rbp;

    backtrace_regs()
        : reg_rip(0), reg_rsp(0), reg_rbp(0) {
    }
    backtrace_regs(uintptr_t rip, uintptr_t rsp, uintptr_t rbp)
        : reg_rip(rip), reg_rsp(rsp), reg_rbp(rbp) {
    }
    backtrace_regs(const regstate& regs)
        : reg_rip(regs.reg_rip), reg_rsp(regs.reg_rsp),
          reg_rbp(regs.reg_rbp) {
    }
};

struct backtracer {
    backtracer(const backtrace_regs& regs, x86_64_pagetable* pt)
        : backtracer(regs, round_up(regs.reg_rsp, PAGESIZE), pt) {
    }
    backtracer(const backtrace_regs& regs, uintptr_t stack_top,
               x86_64_pagetable* pt)
        : rbp_(regs.reg_rbp), rsp_(regs.reg_rsp), stack_top_(stack_top),
          pt_(pt) {
        check_leaf(regs.reg_rip);
        if (!leaf_ && !check()) {
            rbp_ = rsp_ = 0;
        }
    }
    bool ok() const {
        return rsp_ != 0;
    }
    uintptr_t rbp() const {
        return rbp_;
    }
    uintptr_t rsp() const {
        return rsp_;
    }
    uintptr_t ret_rip() const {
        if (leaf_) {
            return deref(rsp_);
        } else {
            return deref(rbp_ + 8);
        }
    }
    void step() {
        if (leaf_) {
            leaf_ = false;
        } else {
            rsp_ = rbp_ + 16;
            rbp_ = deref(rbp_);
        }
        if (!check()) {
            rbp_ = rsp_ = 0;
        }
    }

private:
    uintptr_t rbp_;
    uintptr_t rsp_;
    uintptr_t stack_top_;
    bool leaf_ = false;
    x86_64_pagetable* pt_;

    void check_leaf(uintptr_t rip);
    bool check();

    uintptr_t deref(uintptr_t va) const {
        return *vmiter(pt_, va).kptr<const uintptr_t*>();
    }
};

void backtracer::check_leaf(uintptr_t rip) {
    // Maybe the error happened in a leaf function that had its frame
    // pointer omitted. Use a heuristic to improve the backtrace.

    // “return address” stored in %rsp must be accessible
    if (!(pt_
          && stack_top_ >= rsp_
          && stack_top_ - rsp_ >= 8
          && (rsp_ & 7) == 0
          && vmiter(pt_, rsp_).range_perm(8, PTE_P))) {
        return;
    }

    // “return address” must be near current %rip
    uintptr_t retaddr = deref(rsp_);
    if (intptr_t(retaddr - rip) > 0x10000
        || (retaddr >= stack_top_ - PAGESIZE && retaddr <= stack_top_)) {
        return;
    }

    // instructions preceding “return address” must be a call with known offset
    unsigned char ibuf[5];
    int n;
    vmiter it(pt_, retaddr - 5);
    for (n = 0; n < 5 && it.present(); ++it, ++n) {
        ibuf[n] = *it.kptr<unsigned char*>();
    }
    if (n != 5
        || ibuf[0] != 0xe8 /* `call` */) {
        return;
    }

    // that gives us this function address
    unsigned offset = ibuf[1] + (ibuf[2] << 8) + (ibuf[3] << 16) + (ibuf[4] << 24);
    uintptr_t fnaddr = retaddr + (int) offset;

    // function must be near current %rip
    if (fnaddr > rip
        || rip - fnaddr > 0x1000) {
        return;
    }

    // function prologue must not `push %rbp`
    it.find(fnaddr);
    for (n = 0; n < 5 && it.present(); ++it, ++n) {
        ibuf[n] = *it.kptr<unsigned char*>();
    }
    if (n != 5
        || ibuf[0] == 0x55 /* `push %rbp` */
        || memcmp(ibuf, "\xf3\x0f\x1e\xf1\x55", 5) == 0 /* `endbr64; push %rbp` */) {
        return;
    }

    // ok, we think we got one
    leaf_ = true;
}

bool backtracer::check() {
    // require page table, aligned %rbp, and access to caller frame
    if (!pt_
        || (rbp_ & 7) != 0
        || !vmiter(pt_, rbp_).range_perm(16, PTE_P)) {
        return false;
    }
    // allow stepping from kernel to user; otherwise rbp_ should be >= rsp_
    if (stack_top_ >= VA_HIGHMIN && rbp_ < VA_LOWMAX) {
        stack_top_ = round_up(rbp_, PAGESIZE);
    } else if (rbp_ < rsp_) {
        return false;
    }
    // last check: rbp_ is on the stack ending at stack_top_
    return stack_top_ >= rbp_ && stack_top_ - rbp_ >= 16;
}

__always_inline const backtrace_regs& backtrace_current_regs() {
    // static so we don't use stack space; stack might be full
    static backtrace_regs backtrace_kernel_regs;
    backtrace_kernel_regs.reg_rsp = rdrsp();
    backtrace_kernel_regs.reg_rbp = rdrbp();
    backtrace_kernel_regs.reg_rip = 0;
    return backtrace_kernel_regs;
}

__always_inline x86_64_pagetable* backtrace_current_pagetable() {
    return pa2kptr<x86_64_pagetable*>(rdcr3());
}
}


// error_vprintf
//    Print debugging messages to the console and to the host's
//    `log.txt` file via `log_printf`.

__noinline
void error_vprintf(const char* format, va_list val) {
    error_printer pr;
    pr.vprintf(format, val);
    pr.move_cursor();
}


// check_keyboard
//    Check for the user typing a control key, namely:
//    * 'a': Soft reboot to run p-alice.
//    * 'e': Soft reboot to run p-eve.
//    * 'f': Soft reboot to run p-alice and p-eve.
//    * 'p': Soft reboot to run p-pipewriter and p-pipereader.
//    * 'r': Soft reboot to run p-recurse.
//    * 's': Soft reboot to run p-spawn.
//    * 'q' or Control-C: Exit the virtual machine.
//    Otherwise returns the key typed, or -1 if no key was typed.

int check_keyboard() {
    int c = keyboard_readc();
    if (c == 'a' || c == 'e' || c == 'r' || c == 'f' || c == 's') {
        // Turn off the timer interrupt.
        init_timer(-1);
        // Install a temporary page table to carry us through the
        // process of reinitializing memory. This replicates work the
        // bootloader does.
        x86_64_pagetable* pt = reinterpret_cast<x86_64_pagetable*>(0x1000UL);
        memset(pt, 0, PAGESIZE * 2);
        pt[0].entry[0] = 0x2000 | PTE_P | PTE_W;
        pt[1].entry[0] = PTE_P | PTE_W | PTE_PS;
        wrcr3(reinterpret_cast<uintptr_t>(pt));
        // The soft reboot process doesn't modify memory, so it's
        // safe to pass `multiboot_info` on the kernel stack, even
        // though it will get overwritten as the kernel runs.
        uint32_t multiboot_info[5];
        multiboot_info[0] = 4;
        const char* argument = "friends";
        if (c == 'a') {
            argument = "alice";
        } else if (c == 'e') {
            argument = "eve";
        } else if (c == 'f' || c == 'x') {
            argument = "friends";
        } else if (c == 'p') {
            argument = "pipe";
        } else if (c == 'r') {
            argument = "recurse";
        } else if (c == 's') {
            argument = "spawn";
        }
        uintptr_t argument_ptr = reinterpret_cast<uintptr_t>(argument);
        assert(argument_ptr < 0x100000000L);
        multiboot_info[4] = (uint32_t) argument_ptr;
        // restore initial value of data segment for reboot support
        stash_kernel_data(true);
        extern uint8_t _data_start[], _edata[], _kernel_end[];
        uintptr_t data_size = reinterpret_cast<uintptr_t>(_edata) - reinterpret_cast<uintptr_t>(_data_start);
        uintptr_t zero_size = reinterpret_cast<uintptr_t>(_kernel_end) - reinterpret_cast<uintptr_t>(_edata);
        uint8_t* data_stash = reinterpret_cast<uint8_t*>(SYMTAB_ADDR - data_size);
        memcpy(_data_start, data_stash, data_size);
        memset(_edata, 0, zero_size);
        // restart kernel
        asm volatile("movl $0x2BADB002, %%eax; jmp kernel_entry"
                     : : "b" (multiboot_info) : "memory");
    } else if (c == 0x03 || c == 'q') {
        poweroff();
    }
    return c;
}


// fail
//    Loop until user presses Control-C, then poweroff.

[[noreturn]] void fail() {
    while (true) {
        check_keyboard();
    }
}


// strlcpy_from_user(buf, it, maxlen)
//    Copy a C string from `it` into `buf`. Copies at most `maxlen-1`
//    characters, then null-terminates the string. Stops at first
//    absent or non-user-accessible byte.

void strlcpy_from_user(char* buf, vmiter it, size_t maxlen) {
    size_t i = 0;
    while (i + 1 < maxlen && it.user()) {
        buf[i] = *it.kptr<const char*>();
        ++i, ++it;
    }
    if (i < maxlen) {
        buf[i] = '\0';
    }
}


// panic, proc_panic, user_panic, assert_fail
//    Use console_printf() to print a failure message and then wait for
//    control-C. Also write the failure message to the log.

std::atomic<bool> panicking;

void print_backtrace(printer& pr, const backtrace_regs& regs,
                     x86_64_pagetable* pt, bool exclude_rip = false) {
    backtracer bt(regs, pt);
    if (bt.rsp() != bt.rbp()
        && round_up(bt.rsp(), PAGESIZE) == round_down(bt.rbp(), PAGESIZE)) {
        pr.printf("  warning: possible stack overflow (rsp %p, rbp %p)\n",
                  bt.rsp(), bt.rbp());
    }
    if (!exclude_rip && regs.reg_rip) {
        const char* name;
        if (lookup_symbol(regs.reg_rip, &name, nullptr)) {
            pr.printf("  #0  %p  <%s>\n", regs.reg_rip, name);
        } else {
            pr.printf("  #0  %p\n", regs.reg_rip);
        }
    }
    for (int frame = 1; bt.ok(); bt.step(), ++frame) {
        uintptr_t ret_rip = bt.ret_rip();
        const char* name;
        if (lookup_symbol(ret_rip - 2, &name, nullptr)) {
            pr.printf("  #%d  %p  <%s>\n", frame, ret_rip, name);
        } else {
            pr.printf("  #%d  %p\n", frame, ret_rip);
        }
    }
}

void log_print_backtrace() {
    log_printer pr;
    print_backtrace(pr, backtrace_current_regs(), backtrace_current_pagetable(),
                    true);
}

void log_print_backtrace(const proc* p) {
    log_printer pr;
    print_backtrace(pr, p->regs, p->pagetable);
}

void error_print_backtrace(const regstate& regs, x86_64_pagetable* pt,
                           bool exclude_rip) {
    error_printer pr;
    if (CCOL(cursorpos)) {
        pr.printf("\n");
    }
    print_backtrace(pr, regs, pt, exclude_rip);
    pr.move_cursor();
}

static void vpanic(const backtrace_regs& regs, x86_64_pagetable* pt,
                   const char* format, va_list val) {
    cli();
    panicking = true;
    error_printer pr;
    if (!format) {
        format = "PANIC";
    }
    if (strstr(format, "PANIC") == nullptr) {
        pr.printf("PANIC: ");
    }
    pr.vprintf(format, val);
    if (CCOL(pr.cell_ - console)) {
        pr.printf("\n");
    }
    print_backtrace(pr, regs, pt);
    pr.move_cursor();
}

void panic(const char* format, ...) {
    va_list val;
    va_start(val, format);
    vpanic(backtrace_current_regs(), backtrace_current_pagetable(),
           format, val);
    va_end(val);
    fail();
}

void panic_at(const regstate& regs, const char* format, ...) {
    va_list val;
    va_start(val, format);
    vpanic(regs, backtrace_current_pagetable(), format, val);
    va_end(val);
    fail();
}

void proc_panic(const proc* p, const char* format, ...) {
    va_list val;
    va_start(val, format);
    x86_64_pagetable* pt;
    if ((p->regs.reg_cs & 3) == 0) {
        pt = backtrace_current_pagetable();
    } else {
        pt = p->pagetable;
    }
    vpanic(p->regs, pt, format, val);
    va_end(val);
    fail();
}

void user_panic(const proc* p) {
    const char* fmt;
    char buf[256];
    memset(buf, 0, sizeof(buf));
    if (p->regs.reg_rdi == 0) {
        fmt = "USER PANIC";
    } else {
        strlcpy_from_user(buf, vmiter(p, p->regs.reg_rdi), sizeof(buf));
        fmt = "USER PANIC: %s";
    }
    proc_panic(p, fmt, buf);
}

void assert_fail(const char* file, int line, const char* msg,
                 const char* description) {
    error_printer pr;
    if (description) {
        pr.printf("%s:%d: %s\n", file, line, description);
    }
    pr.printf("%s:%d: kernel assertion '%s' failed\n", file, line, msg);
    print_backtrace(pr, backtrace_current_regs(),
                    backtrace_current_pagetable(), true);
    pr.move_cursor();
    fail();
}


// program_image
//    Types representing program images. For documentation, please see
//    `kernel.hh`.

#define DECLARE_PROCESS_IMAGE_LOCATION(x) \
    extern uint8_t _binary_obj_p_##x##_start[], _binary_obj_p_##x##_end[];
#define DECLARE_PROCESS_IMAGE_RAMIMAGE(x) \
    { #x, _binary_obj_p_##x##_start, _binary_obj_p_##x##_end },


foreach_builtin_process_image(DECLARE_PROCESS_IMAGE_LOCATION)

struct ramimage {
    const char* name;
    void* begin;
    void* end;
} ramimages[] = {
    foreach_builtin_process_image(DECLARE_PROCESS_IMAGE_RAMIMAGE)
};

program_image::program_image(int program_number) {
    elf_ = nullptr;
    if (program_number >= 0
        && size_t(program_number) < sizeof(ramimages) / sizeof(ramimages[0])) {
        elf_ = reinterpret_cast<elf_header*>(ramimages[program_number].begin);
        assert(elf_->e_magic == ELF_MAGIC);
    } else {
        elf_ = nullptr;
    }
}
int program_image::program_number(const char* program_name) {
    for (size_t i = 0; i != sizeof(ramimages) / sizeof(ramimages[0]); ++i) {
        if (strcmp(program_name, ramimages[i].name) == 0) {
            return i;
        }
    }
    return -1;
}
program_image::program_image(const char* program_name)
    : program_image(program_number(program_name)) {
}
uintptr_t program_image::entry() const {
    return elf_ ? elf_->e_entry : 0;
}
bool program_image::empty() const {
    return !elf_ || elf_->e_phnum == 0;
}
static elf_program* elf_header_program(elf_header* elf, bool end) {
    if (elf) {
        elf_program* ph = reinterpret_cast<elf_program*>
            (reinterpret_cast<uint8_t*>(elf) + elf->e_phoff);
        return ph + (end ? elf->e_phnum : 0);
    } else {
        return nullptr;
    }
}
program_image_segment program_image::begin() const {
    return program_image_segment(elf_header_program(elf_, false), elf_);
}
program_image_segment program_image::end() const {
    return program_image_segment(elf_header_program(elf_, true), elf_);
}

inline void program_image_segment::fix() {
    elf_program* eph = elf_header_program(elf_, true);
    while (ph_ != eph && ph_->p_type != ELF_PTYPE_LOAD) {
        ++ph_;
    }
}
program_image_segment::program_image_segment(elf_program* ph, elf_header* elf)
    : ph_(ph), elf_(elf) {
    fix();
}
uintptr_t program_image_segment::va() const {
    return ph_->p_va;
}
size_t program_image_segment::size() const {
    return ph_->p_memsz;
}
const char* program_image_segment::data() const {
    return reinterpret_cast<const char*>(elf_) + ph_->p_offset;
}
size_t program_image_segment::data_size() const {
    return ph_->p_filesz;
}
bool program_image_segment::writable() const {
    return ph_->p_flags & ELF_PFLAG_WRITE;
}
bool program_image_segment::operator==(const program_image_segment& x) const {
    return ph_ == x.ph_;
}
bool program_image_segment::operator!=(const program_image_segment& x) const {
    return ph_ != x.ph_;
}
void program_image_segment::operator++() {
    assert(ph_ != elf_header_program(elf_, true));
    ++ph_;
    fix();
}
void program_image_segment::operator++(int) {
    ++*this;
}


// Functions required by the C++ compiler and calling convention

namespace std {
const nothrow_t nothrow;
}

extern "C" {
// The __cxa_guard functions control the initialization of static variables.

// __cxa_guard_acquire(guard)
//    Return 0 if the static variables guarded by `*guard` are already
//    initialized. Otherwise lock `*guard` and return 1. The compiler
//    will initialize the statics, then call `__cxa_guard_release`.
int __cxa_guard_acquire(long long* arg) {
    std::atomic<char>* guard = reinterpret_cast<std::atomic<char>*>(arg);
    if (guard->load(std::memory_order_relaxed) == 2) {
        return 0;
    }
    while (true) {
        char old_value = guard->exchange(1);
        if (old_value == 2) {
            guard->exchange(2);
            return 0;
        } else if (old_value == 1) {
            pause();
        } else {
            return 1;
        }
    }
}

// __cxa_guard_release(guard)
//    Mark `guard` to indicate that the static variables it guards are
//    initialized.
void __cxa_guard_release(long long* arg) {
    std::atomic<char>* guard = reinterpret_cast<std::atomic<char>*>(arg);
    guard->store(2);
}

// __cxa_pure_virtual()
//    Used as a placeholder for pure virtual functions.
void __cxa_pure_virtual() {
    panic("pure virtual function called in kernel!\n");
}

// __dso_handle, __cxa_atexit
//    Used to destroy global objects at "program exit". We don't bother.
void* __dso_handle;
void __cxa_atexit(...) {
}

}

// stash_kernel_data
//    Soft reboot requires that we restore kernel data memory to
//    its initial state, so we store its initial state in unused
//    physical memory.
static void stash_kernel_data(bool reboot) {
    // stash initial value of data segment for soft-reboot support
    extern uint8_t _data_start[], _edata[], _kernel_end[];
    uintptr_t data_size = reinterpret_cast<uintptr_t>(_edata) - reinterpret_cast<uintptr_t>(_data_start);
    uint8_t* data_stash = reinterpret_cast<uint8_t*>(SYMTAB_ADDR - data_size);
    if (reboot) {
        memcpy(_data_start, data_stash, data_size);
        memset(_edata, 0, _kernel_end - _edata);
    } else {
        memcpy(data_stash, _data_start, data_size);
    }
}


// `proc` members have fixed offsets
static_assert(offsetof(proc, pagetable) == 0, "proc::pagetable has bad offset");
static_assert(offsetof(proc, state) == 12, "proc::state has bad offset");
static_assert(offsetof(proc, regs) == 16, "proc::refs has bad offset");
