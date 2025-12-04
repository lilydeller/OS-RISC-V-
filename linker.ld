/* Linker script for our simple RISC-V OS running on QEMU virt */

OUTPUT_ARCH(riscv)
ENTRY(_start)

/* Define a single RAM region starting at 0x80000000 (QEMU virt) */
MEMORY {
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS {
    /* Code and read-only data go first */
    .text : {
        *(.text.entry)     /* our _start label lives here */
        *(.text*)          /* all other code sections */
    } > RAM

    /* Read-only data (const variables, strings) */
    .rodata : {
        *(.rodata*)
    } > RAM

    /* Initialized data */
    .data : {
        *(.data*)
    } > RAM

    /* Zero-initialized data (BSS) */
    .bss : {
        *(.bss*)
        *(COMMON)
    } > RAM

    /* Align and define a simple stack top symbol */
    . = ALIGN(16);
    PROVIDE(stack_top = . + 0x4000);  /* 16 KB stack */
}
