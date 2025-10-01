#ifndef WEENSYOS_K_APIC_HH
#define WEENSYOS_K_APIC_HH
#include "kernel.hh"

#define MSR_IA32_APIC_BASE 0x1B

struct lapicstate {
    // APIC register IDs
    enum {
        reg_id = 0x02,
        reg_tpr = 0x08,
        reg_svr = 0x0F,
        reg_isr = 0x10,
        reg_tmr = 0x18,
        reg_irr = 0x20,
        reg_esr = 0x28,
        reg_eoi = 0x0B,
        reg_cmci = 0x2F,
        reg_lvt_timer = 0x32,
        reg_lvt_lint0 = 0x35,
        reg_lvt_lint1 = 0x36,
        reg_lvt_error = 0x37,
        reg_icr_low = 0x30,
        reg_icr_high = 0x31,
        reg_timer_initial_count = 0x38,
        reg_timer_current_count = 0x39,
        reg_timer_divide = 0x3E
    };

    enum ipi_type_t {
        ipi_init = 0x500,
        ipi_startup = 0x600
    };

    enum {
        ipi_delivery_status = 0x1000,
        ipi_level_assert = 0x4000,
        ipi_trigger_level = 0x8000,
        ipi_given = 0,
        ipi_self = 0x40000,
        ipi_all = 0x80000,
        ipi_all_excluding_self = 0xC0000
    };

    enum {
        timer_divide_1 = 0x0B,
        timer_periodic = 0x20000
    };

    enum {
        lvt_masked = 0x10000
    };

    static constexpr uint64_t lapic_pa = 0xFEE00000;

    // APICs can't be copied or assigned
    NO_COPY_OR_ASSIGN(lapicstate)

    // return the local APIC for the current CPU
    static inline lapicstate& get();

    // enable this APIC with interrupt number `vector` for spurious IRQs
    inline void enable_lapic(int vector);
    // disable this APIC
    inline void disable_lapic();

    // return this APIC's ID
    inline uint32_t id() const;

    // return the current error state
    inline uint32_t error();

    // acknowledge any outstanding interrupts
    inline void ack();

    // send an IPI to all other processes
    inline void ipi_others(ipi_type_t ipi_type, int vector = 0);
    // return if the previous IPI has not completed
    inline bool ipi_pending() const;

    // read a value from APIC register `reg`
    inline uint32_t read(int reg) const;
    // write `v` to APIC register `reg`
    inline void write(int reg, uint32_t v);

  private:
    struct apic_reg {
        uint32_t v;
        uint32_t padding[3];
    };

    volatile apic_reg reg_[0x40];
};


class ioapicstate {
  public:
    // APIC register IDs
    enum {
        reg_id = 0x0,
        reg_ver = 0x1,
        reg_redtbl = 0x10
    };

    enum {
        redtbl_masked = 0x10000
    };

    static constexpr uint64_t ioapic_pa = 0xFEC00000;

    // APICs can't be copied or assigned
    NO_COPY_OR_ASSIGN(ioapicstate)

    // return the machine's IOAPIC
    static inline ioapicstate& get();

    // return this APIC's ID
    inline uint32_t id() const;

    inline void enable_irq(int entry, int vector, int cpu_apicid);
    inline void disable_irq(int entry);

    // read a value from APIC register `reg`
    inline uint32_t read(int reg) const;
    // write `v` to APIC register `reg`
    inline void write(int reg, uint32_t v);

  private:
    struct apic_reg {
        uint32_t v;
        uint32_t padding[3];
    };

    volatile mutable apic_reg reg_[2];
};


inline lapicstate& lapicstate::get() {
    return *pa2kptr<lapicstate*>(lapic_pa);
}
inline uint32_t lapicstate::id() const {
    return read(reg_id) >> 24;
}
inline void lapicstate::enable_lapic(int vector) {
    write(reg_svr, (read(reg_svr) & ~0xFF) | 0x100 | vector);
}
inline void lapicstate::disable_lapic() {
    write(reg_svr, read(reg_svr) & ~0x100);
}
inline uint32_t lapicstate::error() {
    write(reg_esr, 0);
    return read(reg_esr);
}
inline void lapicstate::ack() {
    write(reg_eoi, 0);
}
inline void lapicstate::ipi_others(ipi_type_t t, int vector) {
    write(reg_icr_low, ipi_all_excluding_self | ipi_level_assert | t | vector);
}
inline bool lapicstate::ipi_pending() const {
    return (read(reg_icr_low) & ipi_delivery_status) != 0;
}
inline uint32_t lapicstate::read(int reg) const {
    return reg_[reg].v;
}
inline void lapicstate::write(int reg, uint32_t v) {
    reg_[reg].v = v;
}

inline ioapicstate& ioapicstate::get() {
    return *pa2kptr<ioapicstate*>(ioapic_pa);
}
inline uint32_t ioapicstate::id() const {
    return read(reg_id) >> 24;
}
inline void ioapicstate::enable_irq(int entry, int vector, int cpu_lapicid) {
    write(reg_redtbl + 2 * entry, vector);
    write(reg_redtbl + 2 * entry + 1, unsigned(cpu_lapicid) << 24);
}
inline void ioapicstate::disable_irq(int entry) {
    write(reg_redtbl + 2 * entry, redtbl_masked);
    write(reg_redtbl + 2 * entry + 1, 0);
}
inline uint32_t ioapicstate::read(int reg) const {
    reg_[0].v = reg;
    return reg_[1].v;
}
inline void ioapicstate::write(int reg, uint32_t v) {
    reg_[0].v = reg;
    reg_[1].v = v;
}

#endif
