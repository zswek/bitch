// kernel/mm/vmm.h

#ifndef _KERNEL_MM_VMM_H_
#define _KERNEL_MM_VMM_H_

#include <asm/types.h>
#include <arch/x86/page.h>

void vmm_init(void);

_size_t get_vmm_free_count(void);

void* vmm_alloc_page(_size_t count);

void vmm_free_page(void* attr, _size_t count);

void* vmm_mmap_page(void* phy_attr);

void* vmm_mmap_pde_page(void* attr, _size_t count, _pde_t* pde);

void vmm_ummap_page(void* attr, _size_t count);

void* dmm_alloc_page(_size_t count);

void dmm_free_page(void* attr, _size_t count);

#endif
