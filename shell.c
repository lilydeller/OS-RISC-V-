#include "shell.h"
#include "uart.h"
#include "tasks.h"
#include "fs.h"

#define LINE_MAX 64

static int str_eq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

static void read_line(char *buf, int max) {
    int i = 0;
    while (1) {
        char c = uart_getc();
        if (c == '\r' || c == '\n') {
            uart_puts("\r\n");
            break;
        } else if (c == '\b' || c == 127) {
            if (i > 0) {
                i--;
                uart_puts("\b \b");
            }
        } else if (i < max - 1) {
            buf[i++] = c;
            uart_putc(c);
        }
    }
    buf[i] = '\0';
}

static void shell_help(void) {
    uart_puts("Commands:\n");
    uart_puts("  help       - show this message\n");
    uart_puts("  ps         - list tasks\n");
    uart_puts("  tick       - run one scheduler round\n");
    uart_puts("  run N      - run N scheduler rounds\n");
    uart_puts("  ls         - list files\n");
    uart_puts("  cat NAME   - print file contents\n");
    uart_puts("  echo TEXT  - print TEXT\n");
}

void shell_run(void) {
    char line[LINE_MAX];

    while (1) {
        uart_puts("> ");
        read_line(line, LINE_MAX);

        if (line[0] == '\0') {
            continue;
        } else if (str_eq(line, "help")) {
            shell_help();
        } else if (str_eq(line, "ps")) {
            tasks_list();
        } else if (str_eq(line, "tick")) {
            tasks_run_once();
        } else if (line[0] == 'r' && line[1] == 'u' && line[2] == 'n' && line[3] == ' ') {
            /* run N rounds */
            int n = 0;
            const char *p = line + 4;
            while (*p >= '0' && *p <= '9') {
                n = n * 10 + (*p - '0');
                p++;
            }
            if (n <= 0) n = 1;
            for (int i = 0; i < n; i++) {
                tasks_run_once();
            }
        } else if (str_eq(line, "ls")) {
            fs_list();
        } else if (line[0] == 'c' && line[1] == 'a' && line[2] == 't' && line[3] == ' ') {
            const char *name = line + 4;
            const char *data = fs_read(name);
            if (data) {
                uart_puts(data);
            } else {
                uart_puts("No such file.\n");
            }
        } else if (line[0] == 'e' && line[1] == 'c' && line[2] == 'h' && line[3] == 'o' && line[4] == ' ') {
            uart_puts(line + 5);
            uart_puts("\n");
        } else {
            uart_puts("Unknown command. Type 'help'.\n");
        }
    }
}
