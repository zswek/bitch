// kernel/mm/alloc.h

#ifndef _KERNEL_MM_ALLOC_H_
#define _KERNEL_MM_ALLOC_H_

void* kmalloc(_size_t len);

void kfree(void *p);

#endif