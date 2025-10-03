#include "k-vmiter.hh"
#include <atomic>

const x86_64_pageentry_t vmiter::zero_pe = 0;

void vmiter::down() {
    while (lbits_ > PAGEOFFBITS && (*pep_ & (PTE_P | PTE_PS)) == PTE_P) {
        perm_ &= *pep_ | ~(PTE_P | PTE_W | PTE_U);
        lbits_ -= PAGEINDEXBITS;
        uintptr_t pa = *pep_ & PTE_PAMASK;
        x86_64_pagetable* pt = reinterpret_cast<x86_64_pagetable*>(pa);
        pep_ = &pt->entry[(va_ >> lbits_) & 0x1FF];
    }
    if ((*pep_ & PTE_PAMASK) >= 0x100000000UL
        && lbits_ < PAGEOFFBITS + 2 * PAGEINDEXBITS) {
        panic("Page table %p may contain uninitialized memory!\n"
              "(Page table contents: %p)\n", pt_, *pep_);
    }
}

void vmiter::find_impl(uintptr_t va, bool stepping) {
    if (stepping && va == 0) {
        lbits_ = done_lbits;
        perm_ = 0;
        pep_ = const_cast<x86_64_pageentry_t*>(&zero_pe);
    } else if (lbits_ < initial_lbits
               && ((va_ ^ va) & (~uintptr_t(0) << (lbits_ + PAGEINDEXBITS))) == 0) {
        // stepping to next entry in current page table level
        int curidx = (reinterpret_cast<uintptr_t>(pep_) % PAGESIZE) >> 3;
        pep_ += ((va >> lbits_) & 0x1FF) - curidx;
    } else if (!va_is_canonical(va)) {
        lbits_ = noncanonical_lbits;
        perm_ = 0;
        pep_ = const_cast<x86_64_pageentry_t*>(&zero_pe);
    } else {
        lbits_ = initial_lbits;
        perm_ = initial_perm;
        pep_ = &pt_->entry[(va >> lbits_) & 0x1FF];
    }
    va_ = va;
    down();
}

void vmiter::next() {
    int lbits = PAGEOFFBITS;
    if (lbits_ > PAGEOFFBITS && !perm()) {
        lbits = lbits_;
    }
    find_impl((va_ | lbits_mask(lbits)) + 1, true);
}

int vmiter::try_map(uintptr_t pa, int perm) {
    if (pa == uintptr_t(-1) && perm == 0) {
        pa = 0;
    }
    // virtual address is page-aligned
    assert((va_ % PAGESIZE) == 0, "vmiter::try_map va not aligned");
    if (perm & PTE_P) {
        // if mapping present, physical address is page-aligned
        assert(pa != uintptr_t(-1), "vmiter::try_map mapping nonexistent pa");
        assert((pa & PTE_PAMASK) == pa, "vmiter::try_map pa not aligned");
    } else {
        assert((pa & PTE_P) == 0, "vmiter::try_map invalid pa");
    }
    // new permissions (`perm`) cannot be less restrictive than permissions
    // imposed by higher-level page tables (`perm_`)
    assert(!(perm & ~perm_ & (PTE_P | PTE_W | PTE_U)));

    while (lbits_ > PAGEOFFBITS && perm) {
        assert(!(*pep_ & PTE_P));
        x86_64_pagetable* pt = static_cast<x86_64_pagetable*>(kalloc(PAGESIZE));
        if (!pt) {
            return -1;
        }
        memset(pt, 0, PAGESIZE);
        std::atomic_thread_fence(std::memory_order_release);
        *pep_ = reinterpret_cast<uintptr_t>(pt) | PTE_P | PTE_W | PTE_U;
        down();
    }

    if (lbits_ == PAGEOFFBITS) {
        std::atomic_thread_fence(std::memory_order_release);
        *pep_ = pa | perm;
    }
    return 0;
}


uint64_t vmiter::range_perm(size_t sz) const {
    uint64_t p = sz > 0 ? perm() : uint64_t(-1);
    if (!p || sz <= range_size()) {
        return p;
    }
    vmiter copy(*this);
    copy.next_range();
    return copy.range_perm_impl(p, sz - range_size());
}

uint64_t vmiter::range_perm_impl(uint64_t p, size_t sz) {
    while (p != 0 && sz > 0) {
        p &= va() ? perm() : 0;
        sz -= min(sz, range_size());
        next_range();
    }
    return p;
}


ptiter::ptiter(x86_64_pagetable* pt)
    : pt_(pt), pep_(&pt_->entry[0]), lbits_(vmiter::initial_lbits), va_(0) {
    down(false);
}

void ptiter::down(bool skip) {
    int stop_lbits = PAGEOFFBITS + PAGEINDEXBITS;
    while (lbits_ < vmiter::done_lbits) {
        if (!skip && (*pep_ & (PTE_P | PTE_PS)) == PTE_P) {
            if (lbits_ == stop_lbits) {
                return;
            }
            lbits_ -= PAGEINDEXBITS;
            uintptr_t pa = *pep_ & PTE_PAMASK;
            x86_64_pagetable* pt = reinterpret_cast<x86_64_pagetable*>(pa);
            pep_ = &pt->entry[(va_ >> lbits_) & 0x1FF];
            continue;
        }
        uintptr_t va = (va_ | vmiter::lbits_mask(lbits_)) + 1;
        if ((va ^ va_) & ~vmiter::lbits_mask(lbits_ + PAGEINDEXBITS)) {
            // up one level
            if (va == 0 && lbits_ == vmiter::initial_lbits) {
                lbits_ = vmiter::done_lbits;
                return;
            }
            stop_lbits = lbits_ + PAGEINDEXBITS;
            lbits_ = vmiter::initial_lbits;
            pep_ = &pt_->entry[(va_ >> lbits_) & 0x1FF];
        } else {
            ++pep_;
            va_ = va_is_canonical(va) ? va : VA_HIGHMIN;
        }
        skip = false;
    }
}
