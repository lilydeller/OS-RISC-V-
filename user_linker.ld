OUTPUT_ARCH(riscv)
ENTRY(_start)
MEMORY {
  USER (rwx) : ORIGIN = 0x80200000, LENGTH = 32M
}
SECTIONS {
  . = ORIGIN(USER);
  .text : { *(.text.entry) *(.text*) } > USER
  .rodata : { *(.rodata*) } > USER
  .data : { *(.data*) } > USER
  .bss : { *(.bss*) *(COMMON) } > USER
  PROVIDE(user_stack_top = ORIGIN(USER) + LENGTH(USER));
}
