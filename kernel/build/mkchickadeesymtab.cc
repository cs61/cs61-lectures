#define _LARGEFILE_SOURCE 1
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "elf.h"
#include "cbyteswap.hh"
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <cassert>
#include <cinttypes>
#include <vector>
#include <random>
#include <algorithm>
#if defined(_MSDOS) || defined(_WIN32)
# include <fcntl.h>
# include <io.h>
#endif

static bool verbose = false;

struct elf_info {
    const char* filename_;
    char* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
    bool changed_ = false;

    bool ok_ = false;
    elf_header* eh_;
    elf_program* pht_;
    elf_section* sht_;
    mutable elf_symbol* symtab_ = nullptr;
    mutable unsigned nsymtab_ = 0;
    mutable const char* symstrtab_;

    void grow(size_t capacity);

    bool validate();
    unsigned find_section(const char* name) const;
    const char* shstrtab() const;
    const char* section_display_name(unsigned i) const;
    uint64_t first_offset() const;
    elf_symbol* symtab() const;
    elf_symbol* find_symbol(const char* name, elf_symbol* after = nullptr) const;
    void sort_symtab();

    void shift_sections(size_t offset, ptrdiff_t diff);
};


void elf_info::grow(size_t capacity) {
    if (capacity > capacity_) {
        capacity = std::max(capacity, capacity_ + 32768);
        char* data = new char[capacity];
        if (capacity_ != 0) {
            memcpy(data, data_, capacity_);
        }
        delete[] data_;
        data_ = data;
        capacity_ = capacity;
        if (ok_) {
            eh_ = reinterpret_cast<elf_header*>(data_);
            pht_ = reinterpret_cast<elf_program*>(data_ + eh_->e_phoff);
            sht_ = reinterpret_cast<elf_section*>(data_ + eh_->e_shoff);
            symtab_ = nullptr;
            nsymtab_ = 0;
        }
    }
}

bool elf_info::validate() {
    if (ok_) {
        return true;
    }

    // basics
    eh_ = reinterpret_cast<elf_header*>(data_);
    if (size_ < sizeof(*eh_)
        || eh_->e_magic != ELF_MAGIC) {
        fprintf(stderr, "%s: not an ELF file\n", filename_);
    clear_exit:
        eh_ = nullptr;
        pht_ = nullptr;
        sht_ = nullptr;
        return false;
    }
    if (eh_->e_phentsize != sizeof(elf_program)
        || eh_->e_shentsize != sizeof(elf_section)) {
        fprintf(stderr, "%s: unexpected component sizes\n", filename_);
        goto clear_exit;
    }
    if (!eh_->e_phnum || !eh_->e_shnum) {
        fprintf(stderr, "%s: empty components\n", filename_);
        goto clear_exit;
    }
    pht_ = reinterpret_cast<elf_program*>(data_ + eh_->e_phoff);
    sht_ = reinterpret_cast<elf_section*>(data_ + eh_->e_shoff);
    if (eh_->e_phoff >= size_
        || (size_ - eh_->e_phoff) / sizeof(*pht_) < eh_->e_phnum
        || eh_->e_shoff >= size_
        || (size_ - eh_->e_shoff) / sizeof(*sht_) < eh_->e_shnum) {
        fprintf(stderr, "%s: bad offsets\n", filename_);
        goto clear_exit;
    }

    // section header string table
    if (eh_->e_shstrndx == 0
        || eh_->e_shstrndx >= eh_->e_shnum
        || sht_[eh_->e_shstrndx].sh_type != ELF_SHT_STRTAB
        || sht_[eh_->e_shstrndx].sh_size == 0) {
        fprintf(stderr, "%s: no section header string table\n", filename_);
        goto clear_exit;
    }
    char* shstrtab = &data_[sht_[eh_->e_shstrndx].sh_offset];
    size_t shstrtab_size = sht_[eh_->e_shstrndx].sh_size;
    if (shstrtab[0] != 0
        || shstrtab[shstrtab_size - 1] != 0) {
        fprintf(stderr, "%s: bad section header string table\n", filename_);
        goto clear_exit;
    }

    // sections
    for (unsigned i = 0; i != eh_->e_shnum; ++i) {
        auto& sh = sht_[i];
        if (i == 0 && sh.sh_type != ELF_SHT_NULL) {
            fprintf(stderr, "%s: should start with null section\n", filename_);
            return false;
        }
        if (sh.sh_name >= shstrtab_size) {
            fprintf(stderr, "%s <#%u>: bad section name\n",
                    filename_, i);
            return false;
        }
        const char* secname = shstrtab + sh.sh_name;
        secname = *secname ? secname : "<empty>";

        if (sh.sh_type != ELF_SHT_NULL
            && sh.sh_type != ELF_SHT_NOBITS
            && (sh.sh_offset >= size_
                || sh.sh_size > size_ - sh.sh_offset)) {
            fprintf(stderr, "%s <%s>: bad offset/size\n",
                    filename_, secname);
            goto clear_exit;
        }
        if (sh.sh_type == ELF_SHT_SYMTAB
            && (sh.sh_link >= eh_->e_shnum
                || sht_[sh.sh_link].sh_type != ELF_SHT_STRTAB)) {
            fprintf(stderr, "%s <%s>: bad linked string table\n",
                    filename_, shstrtab + sh.sh_name);
            return false;
        }
        if (sh.sh_type == ELF_SHT_SYMTAB) {
            elf_symbol* sym = reinterpret_cast<elf_symbol*>
                (data_ + sh.sh_offset);
            size_t size = sh.sh_size / sizeof(*sym);
            size_t strsize = sht_[sh.sh_link].sh_size;
            for (size_t j = 0; j != size; ++j) {
                if (sym[j].st_name >= strsize) {
                    fprintf(stderr, "%s <%s>: symbol name out of range\n",
                            filename_, shstrtab + sh.sh_name);
                    return false;
                }
            }
        }
        if (sh.sh_type == ELF_SHT_STRTAB
            && (sh.sh_size == 0
                || data_[sh.sh_offset] != 0
                || data_[sh.sh_offset + sh.sh_size - 1] != 0)) {
            fprintf(stderr, "%s <%s>: bad string table contents\n",
                    filename_, shstrtab + sh.sh_name);
            return false;
        }
    }

    ok_ = true;
    return true;
}

const char* elf_info::shstrtab() const {
    assert(sht_);
    return &data_[sht_[eh_->e_shstrndx].sh_offset];
}

const char* elf_info::section_display_name(unsigned i) const {
    assert(i < eh_->e_shnum);
    const char* name = shstrtab() + sht_[i].sh_name;
    return *name ? name : "[unnamed]";
}

unsigned elf_info::find_section(const char* name) const {
    assert(ok_);
    auto shstrtab = this->shstrtab();
    for (unsigned i = 0; i != eh_->e_shnum; ++i) {
        if (strcmp(&shstrtab[sht_[i].sh_name], name) == 0) {
            return i;
        }
    }
    return 0;
}

uint64_t elf_info::first_offset() const {
    uint64_t o = ~uint64_t(0);
    for (unsigned i = 0; i != eh_->e_shnum; ++i) {
        if (sht_[i].sh_type != ELF_SHT_NULL
            && sht_[i].sh_type != ELF_SHT_NOBITS) {
            o = std::min(o, sht_[i].sh_offset);
        }
    }
    return o;
}

elf_symbol* elf_info::symtab() const {
    if (!symtab_) {
        unsigned i = find_section(".symtab");
        if (i != 0 && sht_[i].sh_type == ELF_SHT_SYMTAB) {
            symtab_ = reinterpret_cast<elf_symbol*>(data_ + sht_[i].sh_offset);
            nsymtab_ = sht_[i].sh_size / sizeof(elf_symbol);
            auto strtabndx = sht_[i].sh_link;
            symstrtab_ = data_ + sht_[strtabndx].sh_offset;
        }
    }
    return symtab_;
}

static inline bool symbol_less(const elf_symbol& a, const elf_symbol& b) {
    bool asp = (a.st_info & ELF_STT_MASK) > ELF_STT_FUNC;
    bool bsp = (b.st_info & ELF_STT_MASK) > ELF_STT_FUNC;
    if (asp || bsp) {
        return asp && !bsp;
    } else {
        return (a.st_value < b.st_value)
            || (a.st_value == b.st_value
                && a.st_size < b.st_size);
    }
}

void elf_info::sort_symtab() {
    symtab();
    unsigned i = 2;
    while (i < nsymtab_ && !symbol_less(symtab_[i], symtab_[i - 1])) {
        ++i;
    }
    if (i < nsymtab_) {
        if (verbose) {
            fprintf(stderr, "%s: sorting <.symtab>\n", filename_);
            fprintf(stderr, "  `%s` #%u @%" PRIx64 " > `%s` #%u @%" PRIx64 "\n",
                    symstrtab_ + symtab_[i - 1].st_name, i - 1,
                    symtab_[i - 1].st_value,
                    symstrtab_ + symtab_[i].st_name, i,
                    symtab_[i].st_value);
        }
        std::stable_sort(&symtab_[1], &symtab_[nsymtab_], symbol_less);
        changed_ = true;
    }
}

elf_symbol* elf_info::find_symbol(const char* name,
                                  elf_symbol* after) const {
    unsigned i = 0;
    if (after) {
        i = (after - symtab_) + 1;
    }
    symtab();
    while (i < nsymtab_
           && strcmp(symstrtab_ + symtab_[i].st_name, name) != 0) {
        ++i;
    }
    if (i >= nsymtab_) {
        return nullptr;
    } else {
        return &symtab_[i];
    }
}

void elf_info::shift_sections(size_t offset, ptrdiff_t diff) {
    assert(offset < size_ && diff > 0);

    // update program headers
    if (eh_->e_phoff + eh_->e_phnum * sizeof(*pht_) >= offset) {
        fprintf(stderr, "%s: program header spans alignment boundary\n", filename_);
        exit(1);
    }
    for (unsigned i = 0; i != eh_->e_phnum; ++i) {
        auto& ph = pht_[i];
        if (ph.p_offset >= offset) {
            ph.p_offset += diff;
        } else if (ph.p_offset + ph.p_filesz > offset) {
            fprintf(stderr, "%s: program %u spans alignment boundary\n", filename_, i);
            fprintf(stderr, "  shifting %zu+%zu, program %" PRIu64 "+%" PRIu64 "\n",
                    offset, diff, ph.p_offset, ph.p_filesz);
            exit(1);
        }
    }

    // update section headers
    for (unsigned i = 0; i != eh_->e_shnum; ++i) {
        auto& sh = sht_[i];
        if (sh.sh_offset >= offset) {
            sh.sh_offset += diff;
        } else if (sh.sh_type != ELF_SHT_NULL
                   && sh.sh_type != ELF_SHT_NOBITS
                   && sh.sh_offset + sh.sh_size > offset) {
            fprintf(stderr, "%s: section <%s> spans alignment boundary\n", filename_, section_display_name(i));
            fprintf(stderr, "  shifting %zu+%zu, section %" PRIu64 "+%" PRIu64 "\n",
                    offset, diff, sh.sh_offset, sh.sh_size);
            exit(1);
        }
    }

    // move data
    grow(size_ + diff);
    memmove(&data_[offset + diff], &data_[offset], size_ - offset);
    memset(&data_[offset], 0, diff);
    size_ += diff;
    changed_ = true;

    // update main offsets
    if (eh_->e_shoff >= offset) {
        eh_->e_shoff += diff;
        sht_ = reinterpret_cast<elf_section*>(data_ + eh_->e_shoff);
    }
}


static void usage() {
    fprintf(stderr, "Usage: mkchickadeesymtab [-s SYMTABREF] [IMAGE]\n");
    exit(1);
}

static unsigned rewrite_symtabref(elf_info& ei, const char* name,
                                  uint64_t& loadaddr, size_t strtab_off,
                                  size_t size) {
    auto sym = ei.find_symbol(name);
    unsigned nfound = 0;
    while (sym) {
        if ((sym->st_info & ELF_STT_MASK) == ELF_STT_OBJECT
            && sym->st_shndx < ei.eh_->e_shnum) {
            if (sym->st_size != sizeof(elf_symtabref)) {
                fprintf(stderr, "%s: `%s` symbol @0x%" PRIx64 " has size %" PRIu64 " (expected %zu)\n",
                        ei.filename_, name, sym->st_value,
                        sym->st_size, sizeof(elf_symtabref));
                exit(1);
            }

            auto& sh = ei.sht_[sym->st_shndx];
            if (sh.sh_type != ELF_SHT_PROGBITS
                || sym->st_value < sh.sh_addr
                || sym->st_value >= sh.sh_addr + sh.sh_size - sizeof(elf_symtabref)) {
                fprintf(stderr, "%s: `%s` symbol @0x%" PRIx64 " bad reference\n",
                        ei.filename_, name, sym->st_value);
                fprintf(stderr, "  %s addresses [0x%" PRIx64 ", 0x%" PRIx64 ")\n",
                        ei.shstrtab() + sh.sh_name,
                        sh.sh_addr, sh.sh_addr + sh.sh_size);
                exit(1);
            }

            size_t stref_off = sh.sh_offset + (sym->st_value - sh.sh_addr);
            if (!loadaddr) {
                memcpy(&loadaddr, ei.data_ + stref_off, sizeof(loadaddr));
            }
            elf_symtabref stref;
            memcpy(&stref, ei.data_ + stref_off, sizeof(stref));

            elf_symtabref xstref = {
                reinterpret_cast<elf_symbol*>(loadaddr),
                ei.nsymtab_,
                reinterpret_cast<char*>(loadaddr + strtab_off),
                size
            };
            if (memcmp(ei.data_ + stref_off, &xstref, sizeof(xstref)) != 0) {
                memcpy(ei.data_ + stref_off, &xstref, sizeof(xstref));
                ei.changed_ = true;
                if (verbose) {
                    fprintf(stderr, "%s: filling out `%s`\n", ei.filename_, name);
                }
            }
            ++nfound;
        }
        sym = ei.find_symbol(name, sym);
    }
    return nfound;
}

int main(int argc, char** argv) {
    uint64_t loadaddr = 0;
    const char* lsymtab_name = "symtab";
    bool lsymtab_set = false;

    int opt;
    while ((opt = getopt(argc, argv, "a:s:V")) != -1) {
        switch (opt) {
        case 'a': {
            char* end;
            loadaddr = strtoul(optarg, &end, 0);
            if (*end != 0) {
                fprintf(stderr, "bad `-a` argument\n");
                exit(1);
            }
            break;
        }
        case 's': {
            lsymtab_name = optarg;
            lsymtab_set = true;
            break;
        }
        case 'V':
            verbose = true;
            break;
        default:
            usage();
        }
    }

    if (optind + 2 < argc) {
        usage();
    }

    // open and read ELF file
    elf_info ei;

    ei.filename_ = "<stdin>";
    int fd = STDIN_FILENO;
    if (optind < argc && strcmp(argv[optind], "-") != 0) {
        ei.filename_ = argv[optind];
        fd = open(ei.filename_, O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "%s: %s\n", ei.filename_, strerror(errno));
            exit(1);
        }
    }

    mode_t creatmode = S_IRWXU | S_IRWXG | S_IRWXO;
    {
        struct stat s;
        int r = fstat(fd, &s);
        assert(r == 0);
        ei.grow(S_ISREG(s.st_mode) ? (s.st_size + 32767) & ~32767 : 262144);
        creatmode &= s.st_mode;
    }

    while (true) {
        if (ei.size_ == ei.capacity_) {
            ei.grow(ei.capacity_ * 2);
        }
        ssize_t r = read(fd, &ei.data_[ei.size_], ei.capacity_ - ei.size_);
        if (r == 0) {
            break;
        } else if (r == -1 && errno != EAGAIN) {
            fprintf(stderr, "%s: %s\n", ei.filename_, strerror(errno));
            exit(1);
        } else if (r > 0) {
            ei.size_ += r;
        }
    }

    // check contents
    if (!ei.validate()) {
        exit(1);
    }

    // align symbol table
    auto symtabndx = ei.find_section(".symtab");
    if (symtabndx == 0
        || ei.sht_[symtabndx].sh_type != ELF_SHT_SYMTAB) {
        fprintf(stderr, "%s: no .symtab section\n", ei.filename_);
        exit(1);
    }

    {
        auto& symtab = ei.sht_[symtabndx];
        if (symtab.sh_link >= ei.eh_->e_shnum
            || symtab.sh_link != symtabndx + 1
            || ei.sht_[symtab.sh_link].sh_type != ELF_SHT_STRTAB) {
            fprintf(stderr, "%s: .symtab has unexpected links\n", ei.filename_);
            exit(1);
        }

        // align symbol table on a page boundary
        if (symtab.sh_offset & 0xFFF) {
            assert(symtab.sh_addralign < 0x1000);
            symtab.sh_addralign = 0x1000;
            ei.changed_ = true;
            ei.shift_sections(symtab.sh_offset,
                              0x1000 - (symtab.sh_offset & 0xFFF));
            if (verbose) {
                fprintf(stderr, "%s: aligning <.symtab>\n", ei.filename_);
            }
        }
    }

    // figure out allocation range
    uint64_t first_offset = ei.sht_[symtabndx].sh_offset;
    uint64_t strtab_offset = ei.sht_[symtabndx + 1].sh_offset;
    uint64_t last_offset = ei.sht_[symtabndx + 1].sh_offset
        + ei.sht_[symtabndx + 1].sh_size;

    // find `lsymtab_name`
    if (!rewrite_symtabref(ei, lsymtab_name, loadaddr,
                           strtab_offset - first_offset,
                           last_offset - first_offset)
        && lsymtab_set) {
        fprintf(stderr, "%s: no `%s` symbol found\n", ei.filename_, lsymtab_name);
        exit(1);
    }

    // sort symbol table by address
    ei.sort_symtab();

    // mark symbol table as allocated
    if (loadaddr && !(ei.sht_[symtabndx].sh_flags & ELF_SHF_ALLOC)) {
        ei.sht_[symtabndx].sh_flags |= ELF_SHF_ALLOC;
        ei.sht_[symtabndx].sh_addr = loadaddr;
        ei.changed_ = true;
        if (verbose) {
            fprintf(stderr, "%s: marking <.symtab> as allocated\n", ei.filename_);
        }
    }
    if (loadaddr && !(ei.sht_[symtabndx + 1].sh_flags & ELF_SHF_ALLOC)) {
        ei.sht_[symtabndx + 1].sh_flags |= ELF_SHF_ALLOC;
        ei.sht_[symtabndx + 1].sh_addr = loadaddr
            + strtab_offset - first_offset;
        ei.changed_ = true;
        if (verbose) {
            fprintf(stderr, "%s: marking <.symstrtab> as allocated\n", ei.filename_);
        }
    }

    // find or add a program header
    if (loadaddr) {
        unsigned i = 0;
        while (i != ei.eh_->e_phnum
               && ei.pht_[i].p_va + ei.pht_[i].p_filesz < loadaddr) {
            ++i;
        }

        bool found = false;
        if (i != ei.eh_->e_phnum
            && ei.pht_[i].p_va == loadaddr
            && ei.pht_[i].p_offset == first_offset
            && ei.pht_[i].p_filesz >= last_offset - first_offset) {
            found = true;
        }

        if (!found) {
            uint64_t offset0 = ei.first_offset();
            if (ei.eh_->e_phoff + (ei.eh_->e_phnum + 1) * sizeof(*ei.pht_)
                    > offset0
                || ei.eh_->e_shoff <= offset0) {
                fprintf(stderr, "%s: unexpected program headers\n", ei.filename_);
                exit(1);
            }
            auto& ph = ei.pht_[i];
            memmove(&ph + 1, &ph, (ei.eh_->e_phnum - i) * sizeof(ph));
            ++ei.eh_->e_phnum;
            ph.p_type = ELF_PTYPE_LOAD;
            ph.p_flags = ELF_PFLAG_READ;
            ph.p_offset = first_offset;
            ph.p_va = ph.p_pa = loadaddr;
            ph.p_filesz = last_offset - first_offset;
            ph.p_memsz = ph.p_filesz;
            ph.p_align = 0x1000;
            ei.changed_ = true;
            if (verbose) {
                fprintf(stderr, "%s: adding program header\n", ei.filename_);
            }
        }
    }

    // write output
    if (!ei.changed_ && optind + 1 == argc && fd != STDIN_FILENO) {
        exit(0);
    }

    const char* ofn = "-";
    if (optind + 1 == argc) {
        ofn = argv[optind];
    } else if (optind + 2 == argc) {
        ofn = argv[optind + 1];
    }
    int ofd = STDOUT_FILENO;
    if (strcmp(ofn, "-") != 0) {
        ofd = open(ofn, O_WRONLY | O_CREAT | O_TRUNC, creatmode);
        if (ofd == -1) {
            fprintf(stderr, "%s: %s\n", ofn, strerror(errno));
            exit(1);
        }
    }

    size_t off = 0;
    while (off != ei.size_) {
        ssize_t w = write(ofd, &ei.data_[off], ei.size_ - off);
        if (w > 0) {
            off += w;
        } else if (w == -1 && errno != EINTR && errno != EAGAIN) {
            fprintf(stderr, "%s: %s\n", ofn, strerror(errno));
            exit(1);
        }
    }
    exit(0);
}
