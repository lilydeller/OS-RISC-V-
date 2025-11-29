#include <stdint.h>

#define UART0_BASE   0x10000000UL
#define UART_THR     0x00
#define UART_LSR     0x05
#define LSR_TX_IDLE  0x20

static inline void uart_putc(char c) {
    volatile uint8_t *thr = (uint8_t *)(UART0_BASE + UART_THR);
    volatile uint8_t *lsr = (uint8_t *)(UART0_BASE + UART_LSR);
    while ((*lsr & LSR_TX_IDLE) == 0) { }
    *thr = (uint8_t)c;
}

static void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

void kernel_main(void) {
    uart_puts("Hello from minimal RISC-V kernel!\n");
    uart_puts("If you see this, the kernel booted correctly.\n");

    while (1) { }  
}
