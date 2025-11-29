CROSS_COMPILE ?= riscv64-unknown-elf-
CC      = $(CROSS_COMPILE)gcc

# Note the addition of -mcmodel=medany — this fixes the relocation issue
CFLAGS  = -march=rv64imac -mabi=lp64 -nostdlib -nostartfiles \
          -ffreestanding -Wall -Wextra -O2 -mcmodel=medany

SRCS = start.S main.c uart.c
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

all: kernel.elf

kernel.elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) -T linker.ld -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o kernel.elf

run: all
	qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel.elf
