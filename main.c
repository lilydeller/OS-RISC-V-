#include <stdint.h>
#include "uart.h"
#include "fs.h"
#include "tasks.h"
#include "shell.h"

void kernel_main(void) {
    uart_init();
    uart_puts("Booting RISC-V OS demo kernel...\n");

    fs_init();
    tasks_init();
    tasks_register_demo_programs();

    uart_puts("initialization complete. starting shell.\n");

    shell_run();  

    while (1) { }  
}
