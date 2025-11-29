#include "uart.h"
#include "fs.h"

typedef struct {
    const char *name;
    const char *content;
} File;

static File files[] = {
    { "README.md", "This is the RISC-V OS demo filesystem.\n" },
    { "hello.txt", "Hello from your in-memory filesystem!\n" },
    { "manual.txt", "Commands: help, ls, cat <file>, tasks, run <task>\n" }
};

#define FILE_COUNT ((int)(sizeof(files) / sizeof(files[0])))

// simple local string comparison helper
static int str_eq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

void fs_init(void) {
    uart_puts("[FS] initialized with demo files.\n");
}

void fs_list(void) {
    uart_puts("Files:\n");
    for (int i = 0; i < FILE_COUNT; i++) {
        uart_puts("  ");
        uart_puts(files[i].name);
        uart_puts("\n");
    }
}

void fs_cat(const char *filename) {
    for (int i = 0; i < FILE_COUNT; i++) {
        if (str_eq(filename, files[i].name)) {
            uart_puts(files[i].content);
            return;
        }
    }
    uart_puts("No such file.\n");
}
