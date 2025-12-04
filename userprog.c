/* userprog.c - self-contained user program that talks to UART MMIO directly.
 * No external symbols required.
 */

#include <stdint.h>

/* UART MMIO constants used by your kernel too */
#define UART0_BASE   0x10000000UL
#define UART_THR     0x00  /* transmit holding register */
#define UART_LSR     0x05  /* line status register */
#define LSR_TX_IDLE  0x20  /* THR empty */

static inline void uart_putc(char c) {
    volatile uint8_t *thr = (volatile uint8_t *)(UART0_BASE + UART_THR);
    volatile uint8_t *lsr = (volatile uint8_t *)(UART0_BASE + UART_LSR);

    /* wait until UART ready */
    while ((*lsr & LSR_TX_IDLE) == 0) { }

    *thr = (uint8_t)c;
}

static void uart_puts_local(const char *s) {
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
}

/* entry point */
void _start(void) {
    uart_puts_local("Hello from user program at 0x80200000!\n");
    for (;;) {
        /* spin */
    }
}
