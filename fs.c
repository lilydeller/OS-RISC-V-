#include "fs.h"
#include "uart.h"
#include <stddef.h>

#define MAX_FILES  8
#define MAX_NAME   16
#define MAX_DATA   128

typedef struct {
    char name[MAX_NAME];
    char data[MAX_DATA];
    int  used;
} file_t;

static file_t files[MAX_FILES];

static void str_copy(char *dst, const char *src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static int str_eq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

void fs_init(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].used = 0;
    }
    /* seed with one file */
    fs_create("README", "Tiny RISC-V OS demo file.\nUse 'cat README'.\n");
}

int fs_create(const char *name, const char *contents) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            files[i].used = 1;
            str_copy(files[i].name, name, MAX_NAME);
            str_copy(files[i].data, contents, MAX_DATA);
            return 0;
        }
    }
    return -1;
}

const char *fs_read(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && str_eq(files[i].name, name)) {
            return files[i].data;
        }
    }
    return NULL;
}

void fs_list(void) {
    uart_puts("Files:\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            uart_puts("  ");
            uart_puts(files[i].name);
            uart_puts("\n");
        }
    }
}
