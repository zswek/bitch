// kernel/init.h

#ifndef	_KERNEL_INIT_H_
#define	_KERNEL_INIT_H_

#include <asm/types.h>

extern _u8_t _kernel_start[];
extern _u8_t _kernel_end[];

_u8_t* _mem_base_addr;
_size_t _mem_length;

void _kernel_init(void);

#endif