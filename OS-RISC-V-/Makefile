# makefile
# automates building and running through QEMU

CROSS_COMPILE ?= riscv64-unknown-elf-
CC      = $(CROSS_COMPILE)gcc

CFLAGS  = -march=rv64imac -mabi=lp64 -nostdlib -nostartfiles \
          -ffreestanding -Wall -Wextra -O2

all: kernel.elf

kernel.elf: start.o main.o linker.ld
	$(CC) $(CFLAGS) -T linker.ld -o $@ start.o main.o

start.o: start.S
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o kernel.elf

run: all
	qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel.elf
