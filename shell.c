
// This module implements a very simple command-line shell that
// runs on top of the UART serial interface. It allows interactive
// input/output through QEMU’s terminal window and supports basic
// commands for exploring the in-memory filesystem and launching
// demo tasks.
//
// ---------------------------------------------------------------
// Built-in commands:
//   help         - Display this help message
//   ls           - List all files in the filesystem
//   cat <file>   - Display contents of a specific file
//   tasks        - List registered demo tasks
//   run <task>   - Execute a named task
//   load <file>  - Loads a seperate file
//   whoami       - Display current user
//   su           - Switch to superuser (password: riscv)
//   clear        - Clear the screen
//   !!           - Repeat the last command
// ---------------------------------------------------------------
//
// added features:
//   • fake user system (user/root)
//   • persistent command history
//   • clear screen using ANSI escape codes
//   • expanded help and comments for clarity


#include "uart.h"
#include "fs.h"
#include "tasks.h"
#include "shell.h"
#include "loader.h"

#define CMD_BUF_SIZE 64


// Local string helper utilities


// compare two strings for equality (returns 1 if equal, 0 otherwise)
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

// calculate string length
static int str_len(const char *s) {
    int n = 0;
    while (*s++) n++;
    return n;
}


// Simple fake user management system
static const char *current_user = "user";  // default account
static int is_root = 0;                    // 0 = normal, 1 = superuser

// prints the current user identity
static void shell_whoami(void) {
    uart_puts("Current user: ");
    uart_puts(current_user);
    uart_puts(is_root ? " (root)\n" : "\n");
}

// prompts for password and enables superuser mode if correct
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
            uart_putc('*');  // mask input
            buf[i++] = c;
        }
    }
    buf[i] = '\0';

    // check for the hardcoded password
    if (str_eq(buf, "riscv")) {
        is_root = 1;
        current_user = "root";
        uart_puts("Superuser mode enabled.\n");
    } else {
        uart_puts("Authentication failed.\n");
    }
}

static void cmd_quit(void) {
    uart_puts("Exiting shell + halting CPU. Use host to kill QEMU if needed.\n");
    for (;;) { asm volatile("wfi"); }
}

// try and load program from file system and get a pcb, return 0 if fail
static int try_run_file(const char *name) {
    if (!name || name[0] == '\0') return -1;

    uart_puts("Attempting to load file: ");
    uart_puts(name);
    uart_puts("\n");

    pcb_t pcb;
    int r = load_program_from_fs(name, &pcb);
    if (r != 0) {
        uart_puts("loader: failed to load file or not an ELF.\n");
        return -1;
    }

    if (tasks_add_pcb(&pcb) < 0) {
        uart_puts("tasks: out of slots\n");
        return -1;
    }

    tasks_start_program(&pcb);
    return 0;
}

static void cmd_load(const char *arg) {
    if (!arg || arg[0] == '\0') {
        uart_puts("usage: load <file>\n");
        return;
    }

    while (*arg == ' ' || *arg == '\t') arg++;

    if (try_run_file(arg) != 0) {
        uart_puts("load failed: no such ELF or loader error.\n");
    }
}

// Help menu listing all supported commands

static void shell_help(void) {
    uart_puts("Available commands:\n");
    uart_puts("  help         - Show this help message\n");
    uart_puts("  ls           - List files\n");
    uart_puts("  cat <file>   - Display file contents\n");
    uart_puts("  tasks        - List available tasks\n");
    uart_puts("  run <task>   - Run a demo task\n");
    uart_puts("  load <file>  - Load and start an ELF program from the file system\n");
    uart_puts("  quit         - Exit the shell (halts the kernel)\n");
    uart_puts("  whoami       - Show current user (user/root)\n");
    uart_puts("  su           - Become superuser (password: riscv)\n");
    uart_puts("  clear        - Clear the screen\n");
    uart_puts("  !!           - Repeat the last command\n");
}


// Shell main loop
void shell_run(void) {
    char cmd[CMD_BUF_SIZE];
    static char last_cmd[CMD_BUF_SIZE] = {0};

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

        // handle command input
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
        } else if (str_eq(cmd, "whoami")) {
            shell_whoami();
        } else if (str_eq(cmd, "su")) {
            shell_su();
        } else if (str_eq(cmd, "quit") || str_eq(cmd, "exit")) {
            cmd_quit();
        } else if (starts_with(cmd, "load ")) {
            cmd_load(cmd);
        } else if (str_eq(cmd, "clear")) {
            uart_puts("\033[2J\033[H");  // ANSI escape: clear screen + move cursor home
        } else if (str_eq(cmd, "!!")) {
            if (str_len(last_cmd) > 0) {
                uart_puts("Repeating last command: ");
                uart_puts(last_cmd);
                uart_puts("\n");

                // re-run the previous command by copying it into cmd[]
                for (int j = 0; j < CMD_BUF_SIZE; j++)
                    cmd[j] = last_cmd[j];
                continue; // loop back with cmd now equal to last command
            } else {
                uart_puts("No previous command.\n");
            }
        } else if (str_len(cmd) > 0) {
            uart_puts("Unknown command. Type 'help'.\n");
        }

        // store last command (if non-empty)
        if (str_len(cmd) > 0 && !str_eq(cmd, "!!")) {
            for (int j = 0; j < CMD_BUF_SIZE; j++)
                last_cmd[j] = cmd[j];
        }

        uart_puts("> ");
    }
}
