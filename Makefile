# ===============================================================
# RISC-V OS Makefile
# ---------------------------------------------------------------
# Usage:
#   make            → build kernel.elf
#   make run        → build and run in QEMU
#   make clean      → remove build artifacts
# ===============================================================

CROSS   ?= riscv64-unknown-elf-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy

CFLAGS  := -march=rv64imac -mabi=lp64 -nostdlib -nostartfiles -ffreestanding \
           -Wall -Wextra -O2 -mcmodel=medany
LDFLAGS := -T linker.ld

# ---------------------------------------------------------------
# Kernel source files and object files
# ---------------------------------------------------------------
SRCS = start.S main.c uart.c fs.c tasks.c shell.c loader.c start_user.S
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

# ---------------------------------------------------------------
# Build rules
# ---------------------------------------------------------------

# Default build target
all: kernel.elf

# Rule for compiling .c and .S files into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

# ---------------------------------------------------------------
# User program (to be embedded as binary)
# ---------------------------------------------------------------
userprog.elf: userprog.c user_linker.ld
	$(CC) $(CFLAGS) -T user_linker.ld -o $@ userprog.c

userprog_bin.o: userprog.elf
	$(OBJCOPY) -I binary -O elf64-littleriscv -B riscv $< $@

# ---------------------------------------------------------------
# Kernel linking (includes embedded userprog)
# ---------------------------------------------------------------
kernel.elf: $(OBJS) userprog_bin.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) userprog_bin.o

# ---------------------------------------------------------------
# Run and clean
# ---------------------------------------------------------------
run: kernel.elf
	qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel.elf

clean:
	rm -f *.o kernel.elf userprog.elf userprog_bin.o
