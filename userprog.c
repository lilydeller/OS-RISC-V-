/* userprog.c - self-contained user program that talks to UART MMIO directly.
 * No external symbols required.
 */

#include <stdint.h>
#define UART0_BASE 0x10000000UL
#define UART_THR 0x00
#define UART_LSR 0x05
#define LSR_TX_IDLE 0x20

static inline void uart_putc(char c) {
    volatile uint8_t *thr = (volatile uint8_t *)(UART0_BASE + UART_THR);
    volatile uint8_t *lsr = (volatile uint8_t *)(UART0_BASE + UART_LSR);
    while ((*lsr & LSR_TX_IDLE) == 0) {}
    *thr = (uint8_t)c;
}

static void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

void _start(void) {
    uart_puts("Hello from user program at 0x80200000!\n");
    for (;;) {}
}
