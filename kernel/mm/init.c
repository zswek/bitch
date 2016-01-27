// kernel/mm/init.c

#include <kernel/mm/gdt.h>
#include <kernel/mm/mmap.h>

void mm_init(void) {
    
    gdt_init();
    
    mmap_init();
    
}