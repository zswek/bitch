// kernel/process/lock.c

#include <arch/x86/cpu.h>
#include <driver/timer.h>
#include <kernel/process.h>
#include <kernel/process/lock.h>

void process_lock(_lock_t* lock) {
    current_process -> lock = lock;
    current_process -> state = LOCK;
    intr(TIMER_INT);
}

void intr_lock(_lock_t* lock) {
    current_process -> lock = lock;
    while (((current_process == NULL) || (current_process == current_process -> next)) && (*lock == 1)) {
        halt();
    }
    if (*lock == 1) {
        current_process -> state = LOCK;
        intr(TIMER_INT);
    }
}
