// kernel/mm/umm.h

#ifndef _KERNEL_MM_UMM_H_
#define _KERNEL_MM_UMM_H_

#include <asm/types.h>
#include <arch/x86/page.h>

_pte_t get_umm_value(void* attr, _pde_t* pde);

_size_t set_umm_value(void* attr, _pte_t value, _pde_t* pde);

_pde_t* umm_alloc_pde(void);

_size_t umm_mmap_page(void* attr, void* mmap_attr, _pde_t* pde);

void* umm_mmap_kmm(void* attr, _size_t len, _pde_t* pde);

void umm_ummap_kmm(void* attr, _size_t len);

_size_t umm_handler(_pde_t* pde);

void umm_free(_pde_t* pde);

#endif
