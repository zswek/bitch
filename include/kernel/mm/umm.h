// kernel/mm/umm.h

#ifndef _KERNEL_MM_UMM_H_
#define _KERNEL_MM_UMM_H_

#include <bits/types.h>
#include <arch/x86/page.h>

_pde_t* umm_alloc_pde(void);

void* umm_mmap_page(void* attr, void* mmap_attr, _pde_t* pde);

void umm_free(_pde_t* pde);

#endif