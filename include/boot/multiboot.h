// boot/multiboot.h

#ifndef	_BOOT_MULTIBOOT_H
#define	_BOOT_MULTIBOOT_H

#include <asm/types.h>

#define _CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))  

#define _MULTIBOOT_LOAD_MAGIC  0x2BADB002

typedef struct multiboot_t
{
    _u32_t flags;
    _u32_t mem_lower;
    _u32_t mem_upper;
    _u32_t boot_device;
    _u32_t cmdline;
    _u32_t mods_count;
    _u32_t mods_addr;

    struct
    {
        _u32_t num;
        _u32_t size;
        _u32_t addr;
        _u32_t shndx;
    } elf_sec;

    _u32_t mmap_length;
    _u32_t mmap_addr;
    _u32_t drives_length;
    _u32_t drives_addr;
    _u32_t config_table;
    _u32_t boot_loader_name;
    _u32_t apm_table;

    struct
    {
        _u32_t control_info;
        _u32_t mode_info;
        _u32_t mode;
        _u32_t interface_seg;
        _u32_t interface_off;
        _u32_t interface_len;
    } vbe;
}  __attribute__((packed)) _multiboot_t;

#endif