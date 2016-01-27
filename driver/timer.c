// driver/timer.c

#include <arch/x86/io.h>
#include <driver/i8259.h>
#include <driver/timer.h>

#define I8253_C0_IO     0x40
#define I8253_C1_IO     0x41
#define I8253_C2_IO     0x42
#define I8253_CTL_IO    0x43

#define TIMER_LATCH     1193180

static volatile _size_t tick;

static _timer_handler_t timer_handler;

static void irq0_handler(_intr_regs_t *regs) {
    
    if (timer_handler != NULL) {
        timer_handler(regs, tick++);
    }
    
}

_size_t get_timer_ticks(void) {
    
    return tick;
    
}

void set_timer_frequency(_size_t frequency) {
    
    _size_t divisor = TIMER_LATCH / frequency;
    
    outb_p(0x36, I8253_CTL_IO);
    
    outb_p((_u8_t)(divisor & 0xff), I8253_C0_IO);
    outb_p((_u8_t)((divisor >> 8) & 0xff), I8253_C0_IO);
    
}

void reg_timer_handler(_timer_handler_t handler) {
    
    tick = 0;
    
    timer_handler = handler;
    
}

void init_timer(void) {
    
    timer_handler = NULL;
    
    reg_irq_handler(0, irq0_handler);
    
}