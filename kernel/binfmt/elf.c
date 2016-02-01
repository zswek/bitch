// kernel/binfmt/elf.c

#include <asm/types.h>
#include <kernel/def.h>
#include <kernel/mm/alloc.h>
#include <kernel/mm/vmm.h>
#include <kernel/mm/umm.h>
#include <kernel/dev/file.h>
#include <kernel/process.h>
#include <kernel/process/switch.h>
#include <lib/string.h>
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
        if (pheader.p_type == PT_LOAD) {
            _size_t size = (pheader.p_memsz > pheader.p_filesz) ? pheader.p_memsz : pheader.p_filesz;
            void* p = umm_mmap_kmm((void*) pheader.p_vaddr, size, current_process -> pde);
            if (p == NULL) return -1;
            if (file_read(path, p, pheader.p_filesz, pheader.p_offset) == -1) {
                umm_ummap_kmm(p, size);
                return -1;
            }
            memset((void*) ((_size_t) p + pheader.p_filesz), 0, size - pheader.p_filesz);
            umm_ummap_kmm(p, size);
            /*_size_t mmap_attr = pheader.p_vaddr & ~(PAGE_SIZE - 1);
            _size_t page_conut = (pheader.p_vaddr + pheader.p_memsz - mmap_attr + PAGE_SIZE - 1) / PAGE_SIZE;
            void* p = vmm_alloc_page(page_conut);
            if (file_read(path, p + pheader.p_vaddr - mmap_attr, pheader.p_filesz, pheader.p_offset) == -1)
                return -1;
            for (i = 0; i < page_conut; i++) {
                if (umm_mmap_page(p, (void*) (mmap_attr + i * PAGE_SIZE), current_process -> pde) == 0) {
                    vmm_free_page(p, page_conut);
                    return -1;
                }
            }
            vmm_ummap_page(p, page_conut);*/
            debug("ELF(%s): Load Segment, VirtAddr 0x%08X, MemSize 0x%X", path, pheader.p_vaddr, pheader.p_memsz);
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
    (header.e_ident[EI_CLASS] == 0x1) &&
    (header.e_type == 0x2)) {
        if (load_program(path, header.e_phoff, header.e_phentsize, header.e_phnum) != -1) {
            kfree((void*) path);
            debug("ELF(%s): Entry point Address 0x%08X", path, header.e_entry);
            switch_to_user_mode((void*) header.e_entry, (void*) PAGE_OFFSET);
        }
        else {
            debug("ELF(%s): Load Fail", path);
        }
    }
}
