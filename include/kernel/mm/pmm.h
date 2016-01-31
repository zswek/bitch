// kernel/mm/pmm.h

#ifndef _KERNEL_MM_PMM_H_
#define _KERNEL_MM_PMM_H_

#include <bits/types.h>

void pmm_init(void);

_size_t get_pmm_free_count(void);

void* pmm_alloc_page(void);
void* pmm_up_alloc_page(_size_t count);

void pmm_free_page(void* attr, _size_t count);

#endif
