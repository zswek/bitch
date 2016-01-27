// kernel/intr/isr.c

#include <arch/x86/cpu.h>
#include <arch/x86/page.h>
#include <kernel/intr/stub.h>

void isr_handler(_intr_regs_t *regs) {
    
    printk("\nPanic:0x%x\nError Code:0x%x\n%x\n", regs -> int_code, regs -> err_code, get_page_fault_attr());
    
    disable_intr();
    
    halt();
    
}

void isr_int(void) {
    int i;
    for (i = 0; i < 0x14; i++) {
        reg_intr_handler(i, isr_handler);
    }
}