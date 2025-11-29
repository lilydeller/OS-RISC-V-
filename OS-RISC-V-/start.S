// sets up the stack and jumps to your C code
// start.S
    .section .text.entry
    .globl _start
_start:
    // set stack pointer to symbol provided by linker
    la sp, stack_top

    // call C kernel
    call kernel_main

1:  // if kernel_main returns, just spin
    j 1b
