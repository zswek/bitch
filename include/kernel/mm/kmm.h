// kernel/mm/kmm.h

#ifndef _KERNEL_MM_KMM_H_
#define _KERNEL_MM_KMM_H_

#include <bits/types.h>

#include <kernel/def.h>
#include <arch/x86/page.h>

#define get_globl_phy_attr(attr)    (get_globl_page_attr((_u32_t) (attr) / PAGE_SIZE) + (_u32_t) (attr) % PAGE_SIZE)

__attribute__ ((aligned(PAGE_SIZE))) _pde_t kernel_mmap_pde[PAGE_PDE_INDEX];

void globl_mmap_init(void);
void globl_page_mmap(_size_t phy_page_index, _size_t globl_page_index);
void globl_page_remove(_size_t index);
void* get_globl_page_attr(_size_t page_index);
_pte_t* get_globl_pte(_size_t pte_index);

#endif