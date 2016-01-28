// kernel/intr/isr.h

#ifndef _KERNEL_INTR_ISR_H_
#define _KERNEL_INTR_ISR_H_

void reg_user_isr_handler(_intr_handler_t handler);
void isr_int(void);

#endif
