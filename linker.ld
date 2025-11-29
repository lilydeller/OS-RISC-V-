OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS {
    .text : {
        *(.text.entry)
        *(.text*)
    } > RAM

    .rodata : { *(.rodata*) } > RAM
    .data   : { *(.data*) }   > RAM
    .bss    : { *(.bss*) *(COMMON) } > RAM

    . = ALIGN(16);
    PROVIDE(stack_top = . + 0x4000);
}
