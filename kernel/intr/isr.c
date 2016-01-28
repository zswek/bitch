// kernel/intr/isr.c

#include <arch/x86/cpu.h>
#include <arch/x86/page.h>
#include <arch/x86/selector.h>
#include <kernel/def.h>
#include <kernel/intr/stub.h>

static _intr_handler_t user_isr_handler;

static void isr_handler(_intr_regs_t *regs) {

    if ((regs -> cs == selector(GDT_USER_CODE_INDEX, SEL_RPL3)) && user_isr_handler != NULL) {
        user_isr_handler(regs);
    }
    else {
        printk("\nPanic:0x%x\nError Code:0x%x\n%x\n", regs -> int_code, regs -> err_code, get_page_fault_attr());

        disable_intr();

        halt();
    }

}

void reg_user_isr_handler(_intr_handler_t handler) {
    user_isr_handler = handler;
}

void isr_int(void) {

    user_isr_handler = NULL;

    int i;
    for (i = 0; i < 0x14; i++) {
        reg_intr_handler(i, isr_handler);
    }
}
