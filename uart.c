#include "uart.h"

#define UART_BASE   0x10000000UL

/* NS16550-ish register offsets (byte offsets) */
#define UART_RBR    0x00  /* receive buffer (read) */
#define UART_THR    0x00  /* transmit holding (write) */
#define UART_LSR    0x05  /* line status register */

static inline uint8_t mmio_read8(uintptr_t addr) {
    return *(volatile uint8_t *)addr;
}

static inline void mmio_write8(uintptr_t addr, uint8_t val) {
    *(volatile uint8_t *)addr = val;
}

void uart_init(void) {
    // Using QEMU’s default UART config (no setup needed)
}

void uart_putc(char c) {
    // Wait for transmitter to be ready (bit 5)
    while ((mmio_read8(UART_BASE + UART_LSR) & (1 << 5)) == 0);
    mmio_write8(UART_BASE + UART_THR, (uint8_t)c);
}

char uart_getc(void) {
    // Wait for data ready (bit 0)
    while ((mmio_read8(UART_BASE + UART_LSR) & (1 << 0)) == 0);
    return (char)mmio_read8(UART_BASE + UART_RBR);
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

void uart_put_hex(uint64_t v) {
    static const char *digits = "0123456789abcdef";
    uart_puts("0x");
    for (int i = 60; i >= 0; i -= 4)
        uart_putc(digits[(v >> i) & 0xF]);
}

void uart_put_dec(int v) {
    char buf[20];
    int i = 0;

    if (v == 0) {
        uart_putc('0');
        return;
    }
    if (v < 0) {
        uart_putc('-');
        v = -v;
    }
    while (v > 0 && i < (int)sizeof(buf)) {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }
    while (i--)
        uart_putc(buf[i]);
}
