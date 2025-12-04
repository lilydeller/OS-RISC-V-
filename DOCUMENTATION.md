## Documenting the development process
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


### start.S
> Sets the stack pointers (la sp & stack_top) when *_start* is called, calling kernel_main to run forever as a loop listening for input. stack_top refers to the linker file (see below).

### linker.ld
> The linker controls the memory, specifically how the elf kernel lays it out for thinks like entry symbols, artchitecture, RAM regions for physical addresses, etc. 
> After all of this, it calls the before-mentioned stack_top variable and provides it an address to align itself with the program. 

### main.c
> This file calls all of the functionality that is implemented into the command line operating system.
> **uart_init()** --> initializes the UART hardware. To go into more detail, this simply means that any sort of typical actions, like configuring registers, choosing the emulated more, clearing status bits, buffers, etc, are now implemented. After this is called, it returns the drivers for higher level functions that are called later (like uart_puts). 
> **uart_puts()** --> Writing an ASCII string to the console. Also basically puts uart_putc into a loop, where it writes a \r when a \n is reached for neat display.
> **fs_init()** --> file system initialization. Completely in memory. It will create interal data structures to represent file metadata and its content. It will also register empty directory structures, and provide an API (fs_read, fs_list, fs_open, etc) for the shell to call. This lets us use **ls** and **cat**.
> **tasks_init()** --> initializes the task subsystems. This will zero in the task table (which as we learned is just an array of task slots). It will synchronize things like spinlocks and counters. It will set the current scheduler state to -1. It will prepare per-task memory bookkeeping as well. The OS will be ready to register and run tasks after this call. This function is followed by a demo call to "demo"nstrate the tasks.
> After this, we recall the ASCII function to let the user know that everything has been init, then we call the shell run function with an empty while loop to keep it running indefinitely. 


## Seperate Program Functionality

### Loading seperate programs into the operating system
> For this, we will need 2 files: a loader C file and a user linker ld file. The shell will hook these two together, setting the appropriate registers for each to jump to, and having the runnable pcb set sp and jump to entry. I created userprog.c as a simple loadable program that spins indefinetly. 
> **Please bear in mind, this is for fixed-link addresses**. If I have time, I will add relocatable addresses, but the article i'm referencing (listed below) doesnt cover this
> https://baponkar.github.io/Osdev-Notes/09_Loading_Elf/02_Loading_And_Running.html






### At Runtime
> This makeshift operating system runs when QEMU loads the kernal.elf file into memory using the linker.ld providede addresses
> The linker has a _start symbol that lets the CPU know to start execution
> _start is essentially a call to start.S, where it sets the stack top and then calls the main kernel
> The main kernel (kernel_main), initializes everything and transfers control to the user through the console.
> The shell is the switch from 0 to 1, giving the user control through the interactive terminal loop and driving the system. It waits to read, parses the tokens to distinguish built in calls (ls, help, etc) from other files, and advances the scheduler. 
> Speaking of the scheduler, it works in the round-robin fashion instead of preemptive multiasking. In tasks.c, youll see the list of tasks used to expose the step() function of the task. The scheduler then ticks, which lets the main loop know to run a round. \



# Development process of loading seperate programs
So I've created the loader.c and adjusted the fs and tasks files. To create things like the user program, i need a user linker file that will help the OS connect to it. However, a main issue im running into is that its being linked at a high physical adress which fails since its out of range for the compiler. This is how chatgpt described the issue:

"your user program was linked at 0x8020_0000 (a high physical address). The compiler emitted an instruction sequence that assumes the target address fits in the normal “hi20/lo12” relocation range for small models. That failed because the constant (address of the string in .rodata) is out of range for the default code model.
Fix: build the user program with the RISC-V large code model -mcmodel=medany. That tells the compiler/linker to generate code that can reach high addresses (what you want for fixed-address user programs)."

"RISC-V uses relocations that load 32-bit/20-bit immediates into registers; the default small code model assumes code and large constants are within a certain range. When you link at 0x8020_0000, addresses for constants (like string literals) sit outside that range, causing the R_RISCV_HI20 relocation to fail. -mcmodel=medany tells the compiler to emit sequences that can reach arbitrarily located code/data (and the linker to produce correct GOT/reloc usage), so the relocation fits and linking succeeds."

In the user progam at the listed address, we simulate the text output model, rewrite it in the file, and send a quick message when loaded.

You'll need the userprog.elf, so run this command to build it:
riscv64-unknown-elf-gcc -march=rv64imac -mabi=lp64 -mcmodel=medany \
  -nostdlib -static -T user_linker.ld -o userprog.elf userprog.c
  
If you run "riscv64-unknown-elf-readelf -l userprog.elf", youll get some details about the file/program:
 riscv64-unknown-elf-readelf -l userprog.elf

Elf file type is EXEC (Executable file)
Entry point 0x8020009e
There are 3 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  RISCV_ATTRIBUT 0x00000000000010f2 0x0000000000000000 0x0000000000000000
                 0x000000000000004e 0x0000000000000000  R      0x1
  LOAD           0x0000000000001000 0x0000000080200000 0x0000000080200000
                 0x00000000000000e0 0x00000000000000e0  R E    0x1000
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10

 Section to Segment mapping:
  Segment Sections...
   00     .riscv.attributes
   01     .text .rodata
   02


To run this, we ensure that userprog.elf is IN that same folder as everything else. Our fs get file function looks for files by filename, then reads userprog.elf from the disk. WE then run 
"run userprog.elf"

To get the output text of the loaded file!