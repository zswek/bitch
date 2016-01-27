// driver/i8259.h

#ifndef _DRIVER_I8259_H_
#define _DRIVER_I8259_H_

#include <kernel/intr/stub.h>

#define M_INT_VECTOR    0x20
#define S_INT_VECTOR    0x28

#define IRQ0_INT        (M_INT_VECTOR + 0)
#define IRQ1_INT        (M_INT_VECTOR + 1)
#define IRQ2_INT        (M_INT_VECTOR + 2)
#define IRQ3_INT        (M_INT_VECTOR + 3)
#define IRQ4_INT        (M_INT_VECTOR + 4)
#define IRQ5_INT        (M_INT_VECTOR + 5)
#define IRQ6_INT        (M_INT_VECTOR + 6)
#define IRQ7_INT        (M_INT_VECTOR + 7)

#define IRQ8_INT        (S_INT_VECTOR + 0)
#define IRQ9_INT        (S_INT_VECTOR + 1)
#define IRQ10_INT       (S_INT_VECTOR + 2)
#define IRQ11_INT       (S_INT_VECTOR + 3)
#define IRQ12_INT       (S_INT_VECTOR + 4)
#define IRQ13_INT       (S_INT_VECTOR + 5)
#define IRQ14_INT       (S_INT_VECTOR + 6)
#define IRQ15_INT       (S_INT_VECTOR + 7)

void reg_irq_handler(_size_t n, _intr_handler_t handler);
void init_8259A(void);

#endif