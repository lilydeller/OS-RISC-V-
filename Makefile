CROSS   ?= riscv64-unknown-elf-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy

CFLAGS  := -march=rv64imac -mabi=lp64 -nostdlib -nostartfiles -ffreestanding -Wall -Wextra -O2 -mcmodel=medany
LDFLAGS := -T linker.ld

SRCS = start.S main.c uart.c fs.c tasks.c shell.c
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

all: kernel.elf

kernel.elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o kernel.elf

run: kernel.elf
	qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel.elf
