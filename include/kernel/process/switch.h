// kernel/process/switch.h

#ifndef	_KERNEL_PROCESS_SWITCH_H_
#define	_KERNEL_PROCESS_SWITCH_H_

#include <kernel/process.h>

void switch_context(_context_t* prev, _context_t* next);

void switch_to_user_mode(void* entry, void* stack_top);

#endif
