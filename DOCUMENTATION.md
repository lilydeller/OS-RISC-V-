## Documenting the development process![alt text]
    
this project implements a minimal command-line operating system running on a RISC-V virtual machine (`virt` board in QEMU).  it boots from `_start`, initializes UART for input/output, mounts a simple in-memory file system, and launches a shell interface where the user can run commands interactively.


Project Structure
| **start.S** | Assembly entry point. Sets up the stack pointer and jumps to `kernel_main`. |
| **linker.ld** | Defines the memory map (RAM starting at 0x80000000) and stack region for the kernel. |
| **main.c** | The kernel’s main entry. Initializes all subsystems and launches the shell. |
| **uart.c / uart.h** | UART driver for serial input/output to the QEMU console. |
| **fs.c / fs.h** | Implements an in-memory file system with demo files (`README.md`, `hello.txt`, `manual.txt`). |
| **tasks.c / tasks.h** | Implements simple “tasks” — example programs (`counter1`, `counter2`) registered at boot. |
| **shell.c / shell.h** | Command-line interface that handles input and interprets user commands. |
| **string.c / string.h** | Minimal string utilities for comparing and measuring strings. |
| **Makefile** | Automates compilation, linking, and launching under QEMU. |
| **DOCUMENTATION.md** | This file, explaining our process and implementation steps. |


## Development Process

### Step 1: Environment Setup
We installed:
- **RISC-V GNU Toolchain** (`riscv64-unknown-elf-gcc`)
- **QEMU for RISC-V**
- Verified tool versions and configured the PATH so `make` could invoke the cross-compiler.

We created a base folder structure:
riscv-os/
├── start.S
├── main.c
├── uart.c / uart.h
├── linker.ld
├── Makefile

markdown
Copy code

### Bootloader and Kernel Entry
We wrote `start.S` to:
- Define `_start`
- Initialize the stack (`stack_top` from `linker.ld`)
- Call `kernel_main` (C entrypoint)

The `linker.ld` file specified:
- A single RAM region (128MB)
- Sections for `.text`, `.data`, `.bss`, `.rodata`
- Stack alignment and a 16KB stack

### Step 3: UART Communication
`uart.c` handles character-level input and output with:
```c
#define UART0_BASE 0x10000000
Functions implemented:

uart_init()

uart_putc() / uart_getc()

uart_puts() and uart_put_dec()

this enabled terminal I/O over QEMU’s -nographic interface.

File System
We implemented a tiny in-memory FS in fs.c:

c
Copy code
static File files[] = {
  {"README.md", "This is the RISC-V OS demo filesystem.\n"},
  {"hello.txt", "Hello from your in-memory filesystem!\n"},
  {"manual.txt", "Commands: help, ls, cat <file>, tasks, run <task>\n"}
};
This allowed:

ls → list file names

cat <file> → print file contents

Task Management
tasks.c defines a basic system for demo “programs.”
Each task has:

c
Copy code
typedef struct {
    int id;
    const char *name;
    task_step_fn step;
    int active;
} task_t;
Tasks are registered in tasks_register_demo_programs():

c
Copy code
tasks_add("counter1", task_counter1);
tasks_add("counter2", task_counter2);
Users can run them interactively:

csharp
Copy code
> tasks
Available tasks:
  [0] counter1
  [1] counter2
> run counter1
[counter1] tick 0
[counter1] tick 1

Shell Interface
The shell (shell.c) reads commands and executes functions based on input:
Commands implemented:

bash
Copy code
help
ls
cat <file>
tasks
run <task>
whoami
su
We later added:

whoami → prints the current user (root/user)

su → password-based switch to root user (password: riscv)

Compilation and Execution
To compile:

bash
Copy code
make clean
make
To run in QEMU:

bash
Copy code
make run
Output:

shell
Copy code
booting RISC-V OS demo kernel...
[FS] initialized with demo files.
initialization complete. starting shell.
> ls
Files:
  README.md
  hello.txt
  manual.txt
> cat hello.txt
Hello from your in-memory filesystem!

Testing and Debugging
we debugged linker errors, fixed missing symbol references, and verified memory alignment.
after testing, the OS booted correctly, accepted input, and produced stable output.
