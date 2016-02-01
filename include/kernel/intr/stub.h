// kernel/intr/stub.h

#ifndef _KERNEL_INTR_STUB_H_
#define _KERNEL_INTR_STUB_H_

#include <asm/types.h>

typedef
struct intr_regs_t {
    _u32_t ds;
    _u32_t edi;
    _u32_t esi;
    _u32_t ebp;
    _u32_t esp;
    _u32_t ebx;
    _u32_t edx;
    _u32_t ecx;
    _u32_t eax;
    _u32_t int_code;
    _u32_t err_code;
    _u32_t eip;
    _u32_t cs;
    _u32_t eflags;
    _u32_t useresp;
    _u32_t ss;
} __attribute__((packed)) _intr_regs_t;

typedef void (*_intr_handler_t)(_intr_regs_t *);

void reg_intr_handler(_u8_t n, _intr_handler_t handler);

void stub_init(void);

#endif