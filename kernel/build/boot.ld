OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(boot_entry)

SECTIONS {
    . = 0x7c00;

    /* Text segment: instructions only.
       The boot loader must fit in 512 bytes. */
    .text : {
        *(.text.unlikely .text.*_unlikely .text.unlikely.*)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)
    }

    /* Define the locations of shared symbols */
    PROVIDE(console = 0xB8000);
    PROVIDE(cursorpos = 0xB8FFC);

    /DISCARD/ : { *(.eh_frame .note.GNU-stack) }
}
