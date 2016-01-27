// kernel/mm/mmap.h

#ifndef _KERNEL_MM_MMAP_H_
#define _KERNEL_MM_MMAP_H_

#include <bits/types.h>

#include <kernel/def.h>
#include <arch/x86/page.h>

_size_t mem_page_count;

void mmap_init(void);

#endif