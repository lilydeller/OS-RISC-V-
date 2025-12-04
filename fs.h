// fs.h — header file for the simple in-memory filesystem
// this header declares the public interface for the filesystem module (`fs.c`).
// it allows other kernel components (like `shell.c` or `main.c`) to initialize
// the filesystem, list available files, and display their contents.

#ifndef FS_H
#define FS_H
#include <stddef.h>
#include <stdint.h>


//   initializes the in-memory filesystem. in this simple OS, it just prints a
//   message announcing that the filesystem has been set up.
//   called by: - kernel_main() in main.c
void fs_init(void);
//   lists all available files stored in the in-memory filesystem. each file
//   name is printed via the UART driver.
//   called by: - shell command "ls"
void fs_list(void);
//   displays the contents of a specific file. If the given filename exists in
//   the filesystem array, its contents are printed to the console via UART.
//   if not found, prints "No such file."
//   parameters: - filename: the name of the file to display (e.g., "hello.txt")
//   called by: - shell command "cat <filename>"
void fs_cat(const char *filename);
int fs_get_file(const char *name, const uint8_t **data_out, size_t *size_out);

#endif
