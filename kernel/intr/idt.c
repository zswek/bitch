// kernel/intr/idt.c

#include <kernel/def.h>
#include <arch/x86/descriptor.h>
#include <arch/x86/selector.h>

static __attribute__ ((aligned(8))) _gate_descriptor_t idt_list[IDT_LENGTH];

void idt_set(_u8_t n, void* base_attr) {
    if (n < IDT_LENGTH)
        set_gate_descriptor(&idt_list[n], selector(GDT_KERNEL_CODE_INDEX, SEL_RPL0), (_u32_t) base_attr, DESC_P|DESC_D|GATE_TYPE_INT);
}

void idt_init(void) {
    
    load_idt((void*) ((_u32_t) &idt_list + PAGE_OFFSET), IDT_LENGTH);
    
}
