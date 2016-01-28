// kernel/binfmt/elf.c

#include <bits/types.h>
#include <kernel/dev/file.h>
#include <debug.h>

#define EI_NIDENT     16
#define EI_MAG0       0
#define EI_MAG1       1
#define EI_MAG2       2
#define EI_MAG3       3
#define EI_CLASS      4
#define EI_DATA       5
#define EI_VERSION    6
#define EI_OSABI      7
#define EI_ABIVERSION 8
#define EI_PAD        9

#define PT_NULL       0x00000000
#define PT_LOAD       0x00000001
#define PT_DYNAMIC    0x00000002
#define PT_INTERP     0x00000003
#define PT_NOTE       0x00000004
#define PT_SHLIB      0x00000005
#define PT_PHDR       0x00000006
#define PT_LOOS       0x60000000
#define PT_HIOS       0x6fffffff
#define PT_LOPROC     0x70000000
#define PT_HIPROC     0x7fffffff

typedef struct elf32_header_t {
    _u8_t e_ident[EI_NIDENT];
    _u16_t e_type;
    _u16_t e_machine;
    _u32_t e_version;
    _u32_t e_entry;
    _u32_t e_phoff;
    _u32_t e_shoff;
    _u32_t e_flags;
    _u16_t e_ehsize;
    _u16_t e_phentsize;
    _u16_t e_phnum;
    _u16_t e_shentsize;
    _u16_t e_shnum;
    _u16_t e_shstrndx;
} __attribute__((packed)) _elf32_header_t;

typedef struct elf32_program_header_t {
    _u32_t p_type;
    _u32_t p_offset;
    _u32_t p_vaddr;
    _u32_t p_paddr;
    _u32_t p_filesz;
    _u32_t p_memsz;
    _u32_t p_flags;
    _u32_t p_align;
} _elf32_program_header_t;

typedef struct elf32_section_header_t {
    _u32_t sh_name;
    _u32_t sh_type;
    _u32_t sh_flags;
    _u32_t sh_addr;
    _u32_t sh_offset;
    _u32_t sh_size;
    _u32_t sh_link;
    _u32_t sh_info;
    _u32_t sh_addralign;
    _u32_t sh_entsize;
} _elf32_section_header_t;

static _ssize_t load_program(const char* path, _u32_t offect, _u32_t size, _u32_t num) {
    _size_t n;
    for (n = 0; n < num; n++) {
        _elf32_program_header_t pheader;
        if (file_read(path, &pheader, sizeof(_elf32_program_header_t), offect + size * n) == -1) {
            return -1;
        }
    }

    return n;
}

void elf_exec(const char* path) {

    _elf32_header_t header;
    if (file_read(path, &header, sizeof(_elf32_header_t), 0) == -1)
        return;

    if ((header.e_ident[EI_MAG0] == 0x7f) &&
    (header.e_ident[EI_MAG1] == 'E') &&
    (header.e_ident[EI_MAG2] == 'L') &&
    (header.e_ident[EI_MAG3] == 'F') &&
    (header.e_ident[EI_CLASS] == 0x1)) {
        load_program(path, header.e_phoff, header.e_phentsize, header.e_phnum);
    }
}
