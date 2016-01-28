// kernel/mm/vmm.h

#ifndef _KERNEL_MM_VMM_H_
#define _KERNEL_MM_VMM_H_

#include <bits/types.h>

void vmm_init(void);

_size_t get_vmm_free_count(void);

void* vmm_alloc_page(_size_t count);

void vmm_free_page(void* attr, _size_t count);

void* vmm_mmap_page(void* phy_attr);

void vmm_ummap_page(void* attr, _size_t count);

#endif
