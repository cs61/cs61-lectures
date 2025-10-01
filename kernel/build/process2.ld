OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_Z12process_mainv)

PHDRS {
    text PT_LOAD;
    data PT_LOAD;
}

SECTIONS {
    . = 0x140000;

    /* Text segment: instructions and read-only globals */
    .text : {
        *(.text.unlikely .text.*_unlikely .text.unlikely.*)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)
    } :text
    PROVIDE(etext = .);    /* Define the `etext` symbol at this location */
    .rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.data.rel.ro.local* .gnu.linkonce.d.rel.ro.local.*)
        *(.data.rel.ro .data.rel.ro.* .gnu.linkonce.d.rel.ro.*)
    } :text

    /* Data segment: read/write and zero-initialized globals */
    . = ALIGN(4096);       /* Align to a page boundary */
    .data : {
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
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
