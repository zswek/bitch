// arch/x86/descriptor.h

#ifndef _ARCH_X86_DESCRIPTOR_H_
#define _ARCH_X86_DESCRIPTOR_H_

#include <asm/types.h>

#define SEGM_G          0x8000
#define SEGM_DB         0x4000
#define SEGM_L          0x2000
#define SEGM_AVL        0x1000

#define DESC_P          0x80
#define DESC_DPL0       0x00
#define DESC_DPL1       0x20
#define DESC_DPL2       0x40
#define DESC_DPL3       0x60
#define DESC_S          0x10

#define SEGM_TYPE_R     0x00
#define SEGM_TYPE_RW    0x02
#define SEGM_TYPE_RE    0x04
#define SEGM_TYPE_RwE   0x06
#define SEGM_TYPE_C     0x08
#define SEGM_TYPE_CR    0x0a
#define SEGM_TYPE_CC    0x0c
#define SEGM_TYPE_CCR   0x0e

#define DESC_D          0x08

#define SEGM_TYPE_TSS   0x01
#define SEGM_TYPE_LDT   0x02

#define GATE_TYPE_CALL  0x04
#define GATE_TYPE_TASK  0x05
#define GATE_TYPE_INT   0x06
#define GATE_TYPE_TARP  0x07

typedef
struct segment_descriptor_t {
    _u16_t  limit_1;
    _u16_t  base_addr_1;
    _u8_t   base_addr_2;
    _u16_t  attribute;
    _u8_t   base_addr_3;
} __attribute__((packed)) _segment_descriptor_t;

typedef
struct gate_descriptor_t {
    _u16_t  offset_1;
    _u16_t  selector;
    _u8_t   always0;
    _u8_t   attribute;
    _u16_t  offset_2;
} __attribute__((packed)) _gate_descriptor_t;

typedef
struct ptr_t {
    _u16_t limit;
    _u32_t base_attr;
} __attribute__((packed)) _ptr_t;

void set_segment_descriptor (_segment_descriptor_t* descriptor, _u32_t base_attr, _u32_t limit, _u16_t attribute);
void set_gate_descriptor (_gate_descriptor_t* descriptor, _u16_t selector, _u32_t offset, _u8_t attribute);
void load_gdt (void* base_attr, _u16_t length);
void load_idt (void* base_attr, _u16_t length);

#endif