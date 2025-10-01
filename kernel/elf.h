#ifndef WEENSYOS_ELF_H
#define WEENSYOS_ELF_H
#if defined(WEENSYOS_KERNEL) || defined(WEENSYOS_PROCESS)
#include "types.h"
#else
#include <inttypes.h>
#include <stddef.h>
#endif

// elf.h
//
//   Structures and constants for ELF (Executable Linking Format) executable
//   files.

#define ELF_MAGIC 0x464C457FU   // "\x7FELF" in little endian

// executable header
struct elf_header {
    uint32_t e_magic;       // @0 must equal ELF_MAGIC
    uint8_t e_elf[12];
    uint16_t e_type;        // @0x10
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;       // @0x18 entry point: address of first instruction
    uint64_t e_phoff;       // @0x20 offset from elf_header to 1st elf_program
    uint64_t e_shoff;       // @0x28 offset from elf_header to 1st elf_section
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;   // @0x36 should equal sizeof(elf_program)
    uint16_t e_phnum;       // @0x38 number of elf_programs
    uint16_t e_shentsize;   // @0x3a should equal sizeof(elf_section)
    uint16_t e_shnum;       // @0x3c number of elf_sections
    uint16_t e_shstrndx;    // @0x3e
};

// program header (required by the loader)
struct elf_program {
    uint32_t p_type;        // @0x00 see ELF_PTYPE below
    uint32_t p_flags;       // @0x04 see ELF_PFLAG below
    uint64_t p_offset;      // @0x08 offset from elf_header to program data
    uint64_t p_va;          // @0x10 virtual address to load data
    uint64_t p_pa;          // @0x18 not used
    uint64_t p_filesz;      // @0x20 number of bytes of program data
    uint64_t p_memsz;       // @0x28 number of bytes in memory (any bytes beyond
                            //   p_filesz are initialized to zero)
    uint64_t p_align;       // @0x30
};

// section definition (not required by loader)
struct elf_section {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
};

// symbol table entry (not required by loader)
struct elf_symbol {
    uint32_t st_name;
    uint8_t st_info;
    uint8_t st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
};

// in-memory reference to debug symbol table + string table
struct elf_symtabref {
    elf_symbol* sym;
    size_t nsym;
    char* strtab;
    size_t size;
};

// Values for elf_header::e_type
#define ELF_ET_EXEC             2   // executable file

// Values for elf_program::p_type
#define ELF_PTYPE_LOAD          1

// Flag bits for elf_program::p_flags
#define ELF_PFLAG_EXEC          1
#define ELF_PFLAG_WRITE         2
#define ELF_PFLAG_READ          4

// Values for elf_section::sh_type
#define ELF_SHT_NULL            0
#define ELF_SHT_PROGBITS        1
#define ELF_SHT_SYMTAB          2
#define ELF_SHT_STRTAB          3
#define ELF_SHT_NOBITS          8

// Values for elf_section::sh_flags
#define ELF_SHF_ALLOC           2

// Values for elf_symbol::st_name
#define ELF_STN_UNDEF           0

// Values for elf_symbol::st_shndx
#define ELF_SHN_UNDEF           0
#define ELF_SHN_ABS             0xFFF1U
#define ELF_SHN_COMMON          0xFFF2U

// Values for elf_symbol::st_info
#define ELF_STB_MASK            0xF0
#define ELF_STB_LOCAL           0x00
#define ELF_STB_GLOBAL          0x10
#define ELF_STB_WEAK            0x20
#define ELF_STT_MASK            0x0F
#define ELF_STT_OBJECT          0x01
#define ELF_STT_FUNC            0x02

#endif /* !WEENSYOS_ELF_H */
