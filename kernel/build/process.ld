OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(process_main)

PHDRS {
    text PT_LOAD;
    data PT_LOAD;
}

SECTIONS {
    . = 0x100000;

    /* Text segment: instructions and read-only globals */
    .text : {
        *(.text.unlikely .text.*_unlikely .text.unlikely.*)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)
    } :text
    PROVIDE(etext = .);    /* Define the `etext` symbol at this location */

    /* support several signatures for `process_main` */
    process_main = (DEFINED(process_main) ? process_main
        : DEFINED(_Z12process_mainiPPc) ? _Z12process_mainiPPc
        : _Z12process_mainv);

    .rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.data.rel.ro.local* .gnu.linkonce.d.rel.ro.local.*)
        *(.data.rel.ro .data.rel.ro.* .gnu.linkonce.d.rel.ro.*)
    } :text

    /* Constructors: these sections support global initialization
       functions, such as for global C++ objects with constructors. */
    .init_array : {
        PROVIDE(__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*)
                SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array .ctors))
        PROVIDE(__init_array_end = .);
    } :text
    .ctors : {
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    } :text

    /* Data segment: read/write and zero-initialized globals */
    . = ALIGN(4096);       /* Align to a page boundary */
    .data : {
        *(.data .data.* .gnu.linkonce.d.*)
    } :data
    PROVIDE(edata = .);
    .bss : {
        *(.bss .bss.* .gnu.linkonce.b.*)
    } :data
    PROVIDE(end = .);

    /* Define the locations of shared symbols */
    PROVIDE(console = 0xB8000);
    PROVIDE(cursorpos = 0xB8FFC);

    /DISCARD/ : { *(.eh_frame .note.GNU-stack) }
}
