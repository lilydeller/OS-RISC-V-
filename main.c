// this file contains the top-level `kernel_main()` function, which acts as the
// C entry point for the kernel after low-level initialization (performed in
// `start.S`). it initializes hardware interfaces (UART), core OS services
// (filesystem and task manager), and then starts the interactive shell.


#include <stdint.h>
#include "uart.h"
#include "fs.h"
#include "tasks.h"
#include "shell.h"

//   the primary entry point for the OS kernel after boot. this function is
//   called from the `_start` routine defined in `start.S`, once the CPU and
//   stack have been initialized.
//
//   1. initialize the UART hardware so the system can print to the console.
//   2. print a boot message over UART.
//   3. initialize the in-memory filesystem (fs.c).
//   4. initialize the task subsystem (tasks.c).
//   5. register the demo tasks, which can be run via the `run` shell command.
//   6. announce completion and start the interactive command shell (shell.c).
//   7. remain in an infinite loop after the shell is launched.

void kernel_main(void) {
    uart_init();
    uart_puts("booting RISC-V OS demo kernel...\n");

    fs_init();
    tasks_init();
    tasks_register_demo_programs();

    uart_puts("initialization complete. starting shell.\n");

    shell_run();  

    while (1) { }  
}
