// boot/multiboot/init.c

#include <boot/multiboot.h>

#include <kernel/init.h>
#include <kernel/mm.h>
#include <kernel/def.h>

void _multiboot_init(_u32_t magic, _multiboot_t* multiboot_info) {
    
    if (magic != _MULTIBOOT_LOAD_MAGIC) {
    }
    
    if (!(_CHECK_FLAG (multiboot_info->flags, 0))) {
    }
    
    _mem_base_addr = (_u8_t*) (HIGH_ATTR + _kernel_end - _kernel_start);
    _mem_length = multiboot_info -> mem_upper * 1024 + HIGH_ATTR;
    
    _kernel_init();
    
}
