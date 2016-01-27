// arch/x86/descriptor.c

#include <arch/x86/descriptor.h>

#define set_gdt_ptr(ptr)    asm volatile ("lgdt %0": :"m" (ptr))
#define set_idt_ptr(ptr)    asm volatile ("lidt %0": :"m" (ptr))

void set_segment_descriptor (_segment_descriptor_t* descriptor, _u32_t base_attr, _u32_t limit, _u16_t attribute) {
    
    descriptor -> base_addr_1 = base_attr & 0xffff;
    descriptor -> base_addr_2 = (base_attr >> 16) & 0xff;
    descriptor -> base_addr_3 = (base_attr >> 24) & 0xff;
    
    descriptor -> limit_1 = limit & 0xffff;
    
    descriptor -> attribute = (attribute & 0xf0ff) | (limit & 0x0f00);
}

void set_gate_descriptor (_gate_descriptor_t* descriptor, _u16_t selector, _u32_t offset, _u8_t attribute) {
    
    descriptor -> selector = selector & 0xffff;
    
    descriptor -> offset_1 = offset & 0xffff;
    descriptor -> offset_2 = (offset >>16) & 0xffff;
    
    descriptor -> attribute = attribute & 0xff;
    
    descriptor -> always0 = 0x00;
}

void load_gdt (void* base_attr, _u16_t length) {
    
    _ptr_t gdt_ptr;

    gdt_ptr.limit = sizeof(_segment_descriptor_t) * length - 1;
    gdt_ptr.base_attr = (_u32_t) base_attr;
    
    set_gdt_ptr(gdt_ptr);
}

void load_idt (void* base_attr, _u16_t length) {
    
    _ptr_t idt_ptr;

    idt_ptr.limit = sizeof(_gate_descriptor_t) * length - 1;
    idt_ptr.base_attr = (_u32_t) base_attr;
    
    set_idt_ptr(idt_ptr);
}
