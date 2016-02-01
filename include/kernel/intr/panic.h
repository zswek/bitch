// kernel/intr/panic.h

#ifndef _KERNEL_INTR_PANIC_H_
#define _KERNEL_INTR_PANIC_H_

#include <kernel/intr/stub.h>

void panic(_intr_regs_t *regs);

#endif
