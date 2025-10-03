#ifndef WEENSYOS_K_PCI_HH
#define WEENSYOS_K_PCI_HH

struct pcistate {
    // PCI I/O registers
    enum {
        reg_host_bridge_config_addr = 0xCF8,
        reg_host_bridge_config_data = 0xCFC
    };

    // PCI register config offsets
    enum {
        config_vendor = 0,
        config_command = 4,
        config_rpsc = 8,
        config_subclass = 10,
        config_lthb = 12,
        config_bar0 = 16, config_bar1 = 20, config_bar2 = 24,
        config_bar3 = 28, config_bar4 = 32, config_bar5 = 36
    };

    // support at most 8 PCI buses
    static constexpr int addr_end = 0x80000;

    NO_COPY_OR_ASSIGN(pcistate)

    static inline pcistate& get();

    static inline int make_addr(int bus, int slot, int func);
    static inline int addr_bus(int addr);
    static inline int addr_slot(int addr);
    static inline int addr_func(int addr);

    int next(int addr) const;
    template <typename F> int find(F predicate, int addr = 0) const;

    inline uint32_t readl(int addr) const;
    inline uint16_t readw(int addr) const;
    inline uint8_t readb(int addr) const;

    inline void writel(int addr, uint32_t x);
    inline void writew(int addr, uint16_t x);
    inline void writeb(int addr, uint8_t x);

    void enable(int addr);

 private:
    static pcistate state;

    pcistate() = default;
};


inline pcistate& pcistate::get() {
    return state;
}

inline int pcistate::make_addr(int bus, int slot, int func) {
    assert(bus >= 0 && bus < 256);
    assert(slot >= 0 && slot < 32);
    assert(func >= 0 && func < 8);
    return (bus << 16) | (slot << 11) | (func << 8);
}

inline int pcistate::addr_bus(int addr) {
    assert(addr >= 0 && addr < 0x1000000);
    return addr >> 16;
}
inline int pcistate::addr_slot(int addr) {
    assert(addr >= 0 && addr < 0x1000000);
    return (addr >> 11) & 0x1F;
}
inline int pcistate::addr_func(int addr) {
    assert(addr >= 0 && addr < 0x1000000);
    return (addr >> 8) & 0x7;
}

template <typename F>
int pcistate::find(F predicate, int addr) const {
    while (addr >= 0 && !predicate(addr)) {
        addr = next(addr);
    }
    return addr;
}

inline uint32_t pcistate::readl(int addr) const {
    assert(addr >= 0 && !(addr & 3));
    outl(reg_host_bridge_config_addr, 0x80000000U | addr);
    return inl(reg_host_bridge_config_data);
}
inline uint16_t pcistate::readw(int addr) const {
    assert(addr >= 0 && !(addr & 1));
    outl(reg_host_bridge_config_addr, 0x80000000U | addr);
    return inw(reg_host_bridge_config_data + (addr & 3));
}
inline uint8_t pcistate::readb(int addr) const {
    assert(addr >= 0);
    outl(reg_host_bridge_config_addr, 0x80000000U | addr);
    return inb(reg_host_bridge_config_data + (addr & 3));
}

inline void pcistate::writel(int addr, uint32_t data) {
    assert(addr >= 0 && !(addr & 3));
    outl(reg_host_bridge_config_addr, 0x80000000U | addr);
    outl(reg_host_bridge_config_data, data);
}
inline void pcistate::writew(int addr, uint16_t data) {
    assert(addr >= 0 && !(addr & 1));
    outl(reg_host_bridge_config_addr, 0x80000000U | addr);
    outw(reg_host_bridge_config_data, data + (addr & 3));
}
inline void pcistate::writeb(int addr, uint8_t data) {
    assert(addr >= 0);
    outl(reg_host_bridge_config_addr, 0x80000000U | addr);
    outb(reg_host_bridge_config_data, data + (addr & 3));
}

#endif
