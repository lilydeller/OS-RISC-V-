
// this module implements a very simple command-line shell that runs on top of
// the UART serial interface. it allows interactive input/output through QEMU’s
// terminal window and supports basic commands for exploring the in-memory
// filesystem and launching demo tasks.


// commands:
//   help         display all available shell commands
//   ls           list all files in the filesystem
//   cat <file>   display contents of a specific file
//   tasks        list registered demo tasks
//   run <task>   execute a named task

#include "uart.h"
#include "fs.h"
#include "tasks.h"

#define CMD_BUF_SIZE 64

// compare two strings for equality (returns 1 if equal, 0 if not)
static int str_eq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

// check if a string starts with a given prefix
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

static int shell_is_root = 0;

static void shell_whoami(void) {
    if (shell_is_root)
        uart_puts("Current user: root\n");
    else
        uart_puts("Current user: user\n");
}

static void shell_su(void) {
    char buf[CMD_BUF_SIZE];

    uart_puts("Password: ");

    int i = 0;
    while (i < CMD_BUF_SIZE - 1) {
        char c = uart_getc();
        if (c == '\r' || c == '\n') {
            uart_putc('\n');
            break;
        } else if ((c == '\b' || c == 127) && i > 0) {
            i--;
            uart_puts("\b \b");
        } else {
            // don't echo the actual character (just a *)
            uart_putc('*');
            buf[i++] = c;
        }
    }
    buf[i] = '\0';

    // hardcoded password check using our local string helper
    if (str_eq(buf, "riscv")) {
        shell_is_root = 1;
        uart_puts("Superuser enabled.\n");
    } else {
        uart_puts("Authentication failed.\n");
    }
}

// this function provides a built-in help menu listing all supported commands
// it’s printed to the UART terminal when the user types “help”
static void shell_help(void) {
    uart_puts("Available commands:\n");
    uart_puts("  help         - Show this help message\n");
    uart_puts("  ls           - List files\n");
    uart_puts("  cat <file>   - Display file contents\n");
    uart_puts("  tasks        - List available tasks\n");
    uart_puts("  run <task>   - Run a demo task\n");
    uart_puts("  whoami       - Show current user (user/root)\n");
    uart_puts("  su           - Become superuser (root)\n");
}

// process flow:
//   1. display a command prompt ('> ')
//   2. wait for user input one character at a time
//   3. on ENTER, evaluate the full command string
//   4. execute the matching action (help, ls, cat, etc.)
//   5. repeat indefinitely
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
