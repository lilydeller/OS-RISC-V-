<<<<<<< HEAD
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
=======
# OS-RISC-V-
Operating System for a RISC-V computer


# Instructions

In teams of four: Make an operating system for a RISC-V computer. That's right, make a working command line operating system. Try to get as many features as possible. Supposedly, you can do this in Claude (at least it told me it did it). *Supposedly* AI can do everything we can do but that doesn't actually match to what I've seen (https://survey.stackoverflow.co/2025/ai)



Our default AI will be OpenAI's ChatGPT EDU (provided by the school). If you have another tool, you may use one or more costing ~$20 or less/mo.



(Log all chats, I suggest documenting and building a README as you go along)

Step one: Spend about 2 hours generating something, adding as many features as you can:

loading separate programs
running multiple programs simultaneously
synchronization
protection
file system?
How to create/load new programs, etc.
anything else you can think of).
Step two: Get it running in QEMU (5 minutes to 5 hours). For your README, specifically, give the commands to get this up and running on a lab machine.

Step three: Understand what you have (I suggest working together or at least separating the work). One thing I do like about AI is using it to explain code.

Step four: Complete documentation and submit.



(Step 5 will be to add a feature or two, that will be a final small amount of work and program 3).



Try to get it up and running in QEMU using the virt Generic Platform. https://www.qemu.org/docs/master/system/riscv/virt.html . You may have to figure out how to get it running on the lab machines (QEMU not installed afaict).
>>>>>>> 03b963949536012bfdd106f98cbb8e56f68a72a0
