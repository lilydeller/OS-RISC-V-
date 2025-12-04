#include "uart.h"
#include "fs.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
// fs.c — simple in-memory filesystem for RISC-V OS 
// this module implements a minimal filesystem that stores a few "files"
// directly in memory. it allows the shell to use commands like `ls` and `cat`
// without needing disk access or a real storage driver.
//
// supported operations:
//   - fs_init(): initialize and announce the filesystem
//   - fs_list(): list available files
//   - fs_cat(filename): display file contents
//
// the filesystem is completely static and read-only. all file data is stored
// in an in-memory array defined below.

#include "uart.h"
#include "fs.h"

// each file has a name and content stored as constant strings.
// here we just simulate a few files with strings in memory.

typedef struct {
    const char *name;
    const char *content;
    size_t size;
} File;

// these files exist in our simple virtual filesystem.
// they are preloaded at compile time and cannot be modified or deleted.

static File files[] = {
    { "README.md", "This is the RISC-V OS demo filesystem.\n" },
    { "hello.txt", "Hello from your in-memory filesystem!\n" },
    { "manual.txt", "Commands: help, ls, cat <file>, tasks, run <task>\n" }
};

#define FILE_COUNT ((int)(sizeof(files) / sizeof(files[0])))

// this function compares two null-terminated strings (C strings).
// returns 1 if they are equal: 0 otherwise.
// we implement this ourselves since the OS does not have access
// to the C standard library (e.g., strcmp from <string.h>)

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

// prints a simple directory listing to the UART console.
// each file is shown on its own line.

void fs_list(void) {
    uart_puts("Files:\n");
    for (int i = 0; i < FILE_COUNT; i++) {
        uart_puts("  ");
        uart_puts(files[i].name);
        uart_puts("\n");
    }
}

// given a filename, search through the files array.
// if found → print the file's contents.
// if not found → print an error message.
//
// example:
//   > cat hello.txt
//   hello from your in-memory filesystem!

void fs_cat(const char *filename) {
    for (int i = 0; i < FILE_COUNT; i++) {
        if (str_eq(filename, files[i].name)) {
            uart_puts(files[i].content);
            return;
        }
    }
    uart_puts("No such file.\n");
}

// fs_get_file: returns pointer+size for built-in files or embedded userprog. 
//   Returns 0 on success, -1 on not found.
int fs_get_file(const char *name, const uint8_t **data_out, size_t *size_out) {
    for (int i = 0; i < FILE_COUNT; ++i) {
        if (str_eq(name, files[i].name)) {
            *data_out = files[i].content;
            *size_out = files[i].size;
            return 0;
        }
    }
    return -1;
}