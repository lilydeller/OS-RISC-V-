// this header defines the public interface for the shell module.
// the shell provides an interactive command-line environment over UART,
// allowing the user to type commands, explore the in-memory filesystem,
// and run demo tasks in the RISC-V OS.

#ifndef SHELL_H
#define SHELL_H

void shell_run(void);

#endif
