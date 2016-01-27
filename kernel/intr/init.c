// kernel/intr/init.c

#include <arch/x86/cpu.h>
#include <kernel/intr.h>
#include <kernel/intr/idt.h>
#include <kernel/intr/stub.h>
#include <kernel/intr/isr.h>
#include <driver/i8259.h>

void intr_init(void) {
    
    idt_init();
    
    stub_init();
    
    isr_int();
    
    init_8259A();
    
    enable_intr();
    
}