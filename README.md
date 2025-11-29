# Tiny RISC-V OS (CSCE Project)

## Overview

this is a tiny educational operating system for the RISC-V architecture,
designed to run on QEMU's `virt` machine. It was generated and then extended by using ChatGPT, and serves as a starting point for exploring:

- Bare-metal boot on RISC-V
- Memory-mapped I/O via UART
- A simple cooperative scheduler
- Simulated "processes" (programs)
- Basic synchronization via a spinlock
- A toy in-memory file system
- A minimal command-line shell

## Features (mapping to assignment)

- **Loading separate programs**  
  - "Programs" are registered as tasks (`tasks_register_demo_programs`) and can
    be extended by adding new functions and registering them via `tasks_add`.
- **Running multiple programs simultaneously**  
  - The scheduler calls each active task's `step` function round-robin.
- **Synchronization**  
  - Shared `shared_counter` guarded by a spinlock (`lock()` / `unlock()`).
- **Protection**  
  - Logical separation: each task has an ID and only cooperatively accesses
    shared state. (Physical protection/MMU is not implemented but could be a
    future extension.)
- **File system**  
  - In-memory "files" in `fs.c` with `ls` and `cat` commands in the shell.
- **Create/load new programs**  
  - New programs can be added by:  
    1. Writing a new `void myprog_step(void)` function  
    2. Adding it via `tasks_add("myprog", myprog_step)`  
    3. Calling `tick` / `run N` in the shell to schedule them.

## Building on a lab machine

> The exact commands may vary depending on how RISC-V tools are installed in the lab.
> Replace the toolchain prefix if needed (e.g., `riscv64-linux-gnu-`).

1. Clone or copy this directory to your home:

   ```bash
   cd ~/riscv-os
