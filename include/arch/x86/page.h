// arch/x86/page.h

#ifndef _ARCH_X86_PAGE_H_
#define _ARCH_X86_PAGE_H_

#include <bits/types.h>

#define PAGE_MASK       0xfffff000

#define PAGE_PRESENT    0x0001
#define PAGE_WRITE      0x0002
#define PAGE_USER       0x0004
#define PAGE_THROUGH    0x0008
#define PAGE_DIS_CACHE  0x0010
#define PAGE_ACCESSED   0x0020
#define PAGE_PDE_PS     0x0080
#define PAGE_PTE_DIRTY  0x0040
#define PAGE_PTE_PAT    0x0080
#define PAGE_GLOBAL     0x0100

typedef _u32_t _pde_t;
typedef _u32_t _pte_t;

#define load_pde(pde)                   asm volatile ("mov %0, %%cr3" : : "r" (pde))
#define set_page(base_attr, attribute)  (((_u32_t) (base_attr) & PAGE_MASK) | ((attribute) & ~PAGE_MASK))
#define get_page_attr(pte)              ((void*) ((pte) & PAGE_MASK))
#define pde_index(attr)                 ((_u32_t) (attr) >> 22)
#define pte_index(attr)                 (((_u32_t) (attr) & 0x003ff000) >> 12)

#define get_page_fault_attr() ({ \
_u32_t  ret; \
asm volatile ( "mov %%cr2, %0": "=r" (ret)); \
ret; })

#endif