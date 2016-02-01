// kernel/process/process.c

#include <kernel/intr.h>
#include <kernel/mm/vmm.h>
#include <arch/x86/tss.h>
#include <arch/x86/cpu.h>
#include <arch/x86/selector.h>
#include <driver/timer.h>
#include <kernel/process.h>
#include <kernel/process/switch.h>
#include <kernel/mm/kmm.h>
#include <kernel/process/lock.h>
#include <kernel/process/exec.h>
#include <lib/string.h>
#include <debug.h>

extern _process_t init_kernel_stack;

_process_t* current_process = NULL;

static _size_t new_pid = 0;
static _lock_t lock;

static void process_switch(_intr_regs_t *regs, _size_t tick) {
    _process_t* prev = current_process;
    _process_t* last = current_process;
    _process_t* next;
switch_to:
    next = last -> next;
    if (next -> state == RUN) {
        if (next != prev) {
            current_process = next;
            globl_tss.esp0 = (_u32_t) current_process -> stack_top;
            if (prev -> pde != current_process -> pde) {
                load_pde(current_process -> pde);
            }
            switch_context(&(prev -> context), &(current_process -> context));
        }
    }
    else if (next -> state == LOCK) {
        if (*(next -> lock) == 0) {
            *(next -> lock) = 1;
            next -> state = RUN;
        }
        last = next;
        goto switch_to;
    }
    else if (next -> state == WAIT) {
        if (tick >= next -> tick) {
            next -> state = RUN;
        }
        last = next;
        goto switch_to;
    }
    else if (next -> state == EXIT) {
        last -> next = next -> next;
        vmm_free_page(next, KERNEL_STACK_SIZE / PAGE_SIZE);
        goto switch_to;
    }
}

static void init_current_process(void)
{
    current_process = &init_kernel_stack;

    current_process -> state = RUN;
    current_process -> pid = new_pid++;
    current_process -> stack_top = (_size_t*) ((_size_t) current_process + KERNEL_STACK_SIZE);

    current_process -> pde = kernel_mmap_pde;

    current_process -> next = current_process;

    debug("Init Process %d: Stack Address 0x%08X~0x%08X, Page Directory Address 0x%08X", current_process -> pid, current_process, current_process -> stack_top, current_process -> pde);
}

static void init_tss(void) {

    memset(&globl_tss, 0, sizeof(globl_tss));

    globl_tss.ss0 = selector(GDT_KERNEL_DATA_INDEX, SEL_RPL0);
    globl_tss.esp0 = (_u32_t) ((_size_t) current_process + KERNEL_STACK_SIZE);

    globl_tss.cs = selector(GDT_KERNEL_CODE_INDEX, SEL_RPL3);
    globl_tss.ss = globl_tss.ds = globl_tss.es = globl_tss.fs = globl_tss.gs = selector(GDT_KERNEL_DATA_INDEX, SEL_RPL3);

    load_tss(selector(GDT_TSS_INDEX, SEL_RPL3));

}

void process_exit(void) {
    current_process -> state = EXIT;
    debug("Process %d: Exit", current_process -> pid);
wait:
    halt();
    goto wait;
}

_size_t process_create(void (*fn)(void*), void *arg, void (*exit)(void),_pde_t* pde) {

    _process_t* new = vmm_alloc_page(KERNEL_STACK_SIZE / PAGE_SIZE);

    if (new == NULL) return 0;

    new -> state = RUN;
    new -> pid = new_pid++;
    new -> stack_top = (_size_t*) ((_size_t) new + KERNEL_STACK_SIZE);

    *(new -> stack_top - 1) = (_size_t) arg;
    *(new -> stack_top - 2) = (_size_t) exit;
    *(new -> stack_top - 3) = (_size_t) fn;

    new -> context.esp = (_size_t)((_size_t) new -> stack_top - sizeof(_size_t) * 3);

    new -> context.eflags = 0x200;

    new -> pde = pde;

    new -> next = current_process -> next;

    current_process -> next = new;

    debug("Process %d: Entry point Address 0x%08X, Stack Address 0x%08X~0x%08X, Page Directory Address 0x%08X", new -> pid, fn, new, new -> stack_top, new -> pde);
    return new -> pid;
}

void process_init(void) {

    init_timer();

    set_timer_frequency(100);

    init_tss();

    init_current_process();

    init_process_lock(&lock);

    reg_timer_handler(process_switch);

    exec_init();

}
