# this Makefile automates building and running a small RISC-V operating system
# kernel using the RISC-V GNU toolchain it compiles the kernel source files,
# links them with a custom linker script, and runs the final binary in QEMU.
# usage:
#   make            build the kernel ELF
#   make run        build and launch in QEMU
#   make clean      remove all compiled output files


# tool definitions
CROSS   ?= riscv64-unknown-elf-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy

CFLAGS  := -march=rv64imac -mabi=lp64 -nostdlib -nostartfiles -ffreestanding -Wall -Wextra -O2 -mcmodel=medany
LDFLAGS := -T linker.ld

# SRCS — list of all kernel source files
# OBJS — automatically convert each .c and .S file to a corresponding .o file
SRCS = start.S main.c uart.c fs.c tasks.c shell.c loader.c start_user.S
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

# this links all object files into the final kernel.elf binary that QEMU runs
all: kernel.elf

kernel.elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

# %.o: %.c — compile a .c file into an object file
# %.o: %.S — assemble an .S file into an object file
# $< — the first prerequisite (the input file)
# $@ — the target (the output file)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o kernel.elf

run: kernel.elf
	qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel.elf
