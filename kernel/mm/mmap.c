// kernel/mm/mmap.c

#include <kernel/init.h>
#include <kernel/mm/kmm.h>
#include <arch/x86/page.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/mm/mmap.h>

void mm_flush(void);

void mmap_init(void) {

    mem_page_count = _mem_length / PAGE_SIZE;

    pmm_init();

    globl_mmap_init();

    vmm_init();

    load_pde(kernel_mmap_pde);

    _size_t kernel_page_num = ((_size_t) _kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;

    int i;
    for (i = 0; i <= kernel_page_num / PAGE_PTE_INDEX; i++)
        kernel_mmap_pde[i] = set_page(get_globl_pte(i),
        PAGE_PRESENT|PAGE_WRITE);

    mm_flush();

    for (i = 0; i <= kernel_page_num / PAGE_PTE_INDEX; i++)
        kernel_mmap_pde[i] = 0;

}
