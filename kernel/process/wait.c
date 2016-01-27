// kernel/process/wait.c

#include <arch/x86/cpu.h>
#include <kernel/process.h>
#include <driver/timer.h>

void process_wait(_size_t tick) {
    current_process -> tick = get_timer_ticks() + tick;
    current_process -> state = WAIT;
    intr(TIMER_INT);
}
