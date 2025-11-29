.section .text.entry
.globl _start
_start:
    la sp, stack_top
    call kernel_main
1:  j 1b
