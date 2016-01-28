// kernel/mm/kmm.c

#include <kernel/init.h>
#include <kernel/mm/mmap.h>
#include <arch/x86/page.h>
#include <kernel/mm/kmm.h>
#include <debug.h>

#define GLOBL_PTE_NUM       (GLOBL_PAGE_NUM / PAGE_PTE_INDEX)

static __attribute__ ((aligned(PAGE_SIZE))) _pte_t globl_mmap_pte[GLOBL_PTE_NUM][PAGE_PTE_INDEX];

void globl_mmap_init(void) {

    int m, n;

    for (m = 0; m < PAGE_PDE_INDEX; m++) {
        kernel_mmap_pde[m] = set_page(0, 0);
    }

    for (m = 0; m < GLOBL_PTE_NUM; m++) {
        kernel_mmap_pde[pde_index(PAGE_OFFSET) + m] = set_page(&globl_mmap_pte[m],
        PAGE_PRESENT|PAGE_WRITE);
        for (n = 0; n < PAGE_PTE_INDEX; n++) {
            globl_mmap_pte[m][n] = set_page(0, 0);
        }
    }

    debug("Kernel Mapp: %d pages, Start Address 0x%08X", GLOBL_PAGE_NUM, PAGE_OFFSET);
}

void globl_page_mmap(_size_t phy_page_index, _size_t globl_page_index) {
    if (globl_page_index < GLOBL_PTE_NUM * PAGE_PTE_INDEX)
        globl_mmap_pte[0][globl_page_index] =
        set_page(phy_page_index << 12, PAGE_PRESENT|PAGE_WRITE);
}

void globl_page_remove(_size_t index) {
    if (index < GLOBL_PTE_NUM * PAGE_PTE_INDEX)
        globl_mmap_pte[0][index] = set_page(0, 0);
}

void* get_globl_page_attr(_size_t page_index) {
    return (void*) get_page_attr(globl_mmap_pte[0][page_index]);
}

_pte_t* get_globl_pte(_size_t pte_index) {
    return &globl_mmap_pte[pte_index][0];
}
