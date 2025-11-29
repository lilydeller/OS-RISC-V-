#include "uart.h"
#include "fs.h"
#include "tasks.h"

#define CMD_BUF_SIZE 64

// tiny local string helpers
static int str_eq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

static int starts_with(const char *s, const char *prefix) {
    while (*prefix) {
        if (*s != *prefix) return 0;
        s++;
        prefix++;
    }
    return 1;
}

static int str_len(const char *s) {
    int n = 0;
    while (*s++) n++;
    return n;
}

static void shell_help(void) {
    uart_puts("Available commands:\n");
    uart_puts("  help         - Show this help message\n");
    uart_puts("  ls           - List files\n");
    uart_puts("  cat <file>   - Display file contents\n");
    uart_puts("  tasks        - List available tasks\n");
    uart_puts("  run <task>   - Run a demo task\n");
}

void shell_run(void) {
    char cmd[CMD_BUF_SIZE];

    uart_puts("> ");
    while (1) {
        int i = 0;

        // read a line into cmd[]
        while (i < CMD_BUF_SIZE - 1) {
            char c = uart_getc();
            if (c == '\r' || c == '\n') {
                uart_putc('\n');
                cmd[i] = '\0';
                break;
            } else if ((c == '\b' || c == 127) && i > 0) {
                i--;
                uart_puts("\b \b");
            } else {
                uart_putc(c);
                cmd[i++] = c;
            }
        }
        cmd[i] = '\0';

        // handle commands
        if (str_eq(cmd, "help")) {
            shell_help();
        } else if (str_eq(cmd, "ls")) {
            fs_list();
        } else if (starts_with(cmd, "cat ")) {
            fs_cat(cmd + 4);
        } else if (str_eq(cmd, "tasks")) {
            tasks_list();
        } else if (starts_with(cmd, "run ")) {
            tasks_run(cmd + 4);
        } else if (str_len(cmd) > 0) {
            uart_puts("Unknown command. Type 'help'.\n");
        }

        uart_puts("> ");
    }
}
