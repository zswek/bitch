// driver/timer.h

#ifndef _DRIVER_TIMER_H_
#define _DRIVER_TIMER_H_

#include <bits/types.h>
#include <kernel/intr/stub.h>
#include <driver/i8259.h>

#define TIMER_INT       IRQ0_INT

typedef void (*_timer_handler_t)(_intr_regs_t *, _size_t);

_size_t get_timer_ticks(void);
void set_timer_frequency(_size_t frequency);

void reg_timer_handler(_timer_handler_t handler);
void init_timer(void);

#endif