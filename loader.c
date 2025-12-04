// loader.c - copy PT_LOAD segments from embedded buffer into memory (no libc)
#include "uart.h"
#include "fs.h"
#include "tasks.h"
#include <stdint.h>
#include <stddef.h>

#define ELF_MAGIC0 0x7f
#define ELF_MAGIC1 'E'
#define ELF_MAGIC2 'L'
#define ELF_MAGIC3 'F'

#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define EM_RISCV 243

#define PT_LOAD 1

typedef struct {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} Elf64_Phdr;

// match the user link and define the user region
#ifndef USER_BASE
#define USER_BASE 0x80200000UL
#endif
#ifndef USER_SIZE
#define USER_SIZE (32 * 1024 * 1024)
#endif

static int address_in_user_region(uint64_t vaddr, uint64_t memsz) {
    if (vaddr < USER_BASE) return 0;
    if (vaddr + memsz > USER_BASE + USER_SIZE) return 0;
    return 1;
}


static void *mini_memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < n; ++i) d[i] = s[i];
    return dst;
}
static void *mini_memset(void *dst, int c, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    for (size_t i = 0; i < n; ++i) d[i] = (uint8_t)c;
    return dst;
}


int load_program_from_fs(const char *path, pcb_t *out_pcb) {
    const uint8_t *buf = NULL;
    size_t size = 0;
    if (fs_get_file(path, &buf, &size) != 0) {
        uart_puts("loader: file not found in FS\n");
        return -1;
    }
    if (size < sizeof(Elf64_Ehdr)) {
        uart_puts("loader: file too small\n");
        return -1;
    }

    const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *)buf;
    if (ehdr->e_ident[0] != ELF_MAGIC0 || ehdr->e_ident[1] != ELF_MAGIC1 ||
        ehdr->e_ident[2] != ELF_MAGIC2 || ehdr->e_ident[3] != ELF_MAGIC3) {
        uart_puts("loader: not ELF\n");
        return -1;
    }
    if (ehdr->e_ident[4] != ELFCLASS64 || ehdr->e_ident[5] != ELFDATA2LSB) {
        uart_puts("loader: wrong ELF class/endian\n");
        return -1;
    }
    if (ehdr->e_machine != EM_RISCV) {
        uart_puts("loader: not RISC-V ELF\n");
        return -1;
    }


    uint64_t ph_end = ehdr->e_phoff + (uint64_t)ehdr->e_phnum * ehdr->e_phentsize;
    if (ph_end > size) {
        uart_puts("loader: phdrs out of range\n");
        return -1;
    }

    const Elf64_Phdr *phdrs = (const Elf64_Phdr *)(buf + ehdr->e_phoff);
    for (uint16_t i = 0; i < ehdr->e_phnum; ++i) {
        const Elf64_Phdr *ph = &phdrs[i];
        if (ph->p_type != PT_LOAD) continue;

        if (ph->p_offset + ph->p_filesz > size) {
            uart_puts("loader: segment truncated\n");
            return -1;
        }
        if (!address_in_user_region(ph->p_vaddr, ph->p_memsz)) {
            uart_puts("loader: segment out of user region\n");
            return -1;
        }

        uint8_t *dst = (uint8_t *)(uintptr_t)ph->p_vaddr;
        mini_memcpy(dst, buf + ph->p_offset, (size_t)ph->p_filesz);
        if (ph->p_memsz > ph->p_filesz) {
            mini_memset(dst + ph->p_filesz, 0, (size_t)(ph->p_memsz - ph->p_filesz));
        }
    }

    out_pcb->entry = (uint64_t)ehdr->e_entry;
    if (tasks_alloc_stack(out_pcb) != 0) {
        uart_puts("loader: no stack\n");
        return -1;
    }
    out_pcb->state = TASK_RUNNABLE;
    return 0;
}
