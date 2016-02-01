// kernel/intr/idt.h

#ifndef _KERNEL_INTR_IDT_H_
#define _KERNEL_INTR_IDT_H_

#include <asm/types.h>

void idt_set(_u8_t n, void* base_attr);

void idt_attribute(_u8_t n, _u8_t attribute);

void idt_init(void);

#endif
