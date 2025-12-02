* this file tells the linker (ld) exactly how to place the compiled sections
* of program (code, data, etc.) into memory. it defines:
* the memory map for the RISC-V “virt” machine in QEMU
* where execution begins (the entry point)
* how sections like .text, .data, and .bss are laid out in RAM

* OUTPUT_ARCH specifies the target architecture: RISC-V.
* ENTRY defines the symbol where execution starts (found in start.S)

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
