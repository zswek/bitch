// kernel/intr/panic.c

#include <arch/x86/cpu.h>
#include <arch/x86/page.h>
#include <kernel/intr/stub.h>
#include <lib/printk.h>

void panic(_intr_regs_t *regs) {

    printk("\nPanic: 0x%08X\nError Code: 0x%08X\n", regs -> int_code, regs -> err_code);

    if (regs -> int_code == 0xe) {
        printk("Fault Address: 0x%08X\n", get_page_fault_attr());
    }

    disable_intr();

    halt();
}
