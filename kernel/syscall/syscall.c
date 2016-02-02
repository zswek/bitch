// kernel/syscall/syscall.c

#include <arch/x86/cpu.h>
#include <arch/x86/descriptor.h>
#include <asm/unistd.h>
#include <kernel/def.h>
#include <kernel/intr/idt.h>
#include <kernel/intr/stub.h>
#include <kernel/process.h>
#include <kernel/syscall.h>
#include <kernel/syscall/io.h>
#include <kernel/syscall/process.h>
#include <debug.h>

static void syscall_handler(_intr_regs_t *regs) {

    enable_intr();

    debug("Process %d: SysCall %d", current_process -> pid, regs -> eax);

    switch (regs -> eax) {
        case __SYS_exit:
            sys_exit();
            break;
        case __SYS_exec:
            regs -> eax = sys_exec((char *) regs -> ebx, (_size_t) regs -> ecx);
            break;
        case __SYS_get:
            regs -> eax = sys_get((char *) regs -> ebx, (_size_t) regs -> ecx);
            break;
        case __SYS_put:
            regs -> eax = sys_put((char *) regs -> ebx, (_size_t) regs -> ecx);
            break;
    }

    disable_intr();
}

void syscall_init(void) {

    idt_attribute(SYSCALL_INT, DESC_DPL3);

    reg_intr_handler(SYSCALL_INT, syscall_handler);
}
