// arch/x86/tss.h

#ifndef _ARCH_X86_TSS_H_
#define _ARCH_X86_TSS_H_

#include <bits/types.h>

typedef
struct task_state_segment_t {
    _u16_t  link;
    _u16_t  reserved_1;
    _u32_t  esp0;
    _u16_t  ss0;
    _u16_t  reserved_2;
    _u32_t  esp1;
    _u16_t  ss1;
    _u16_t  reserved_3;
    _u32_t  esp2;
    _u16_t  ss2;
    _u16_t  reserved_4;
    _u32_t  cr3;
    _u32_t  eip;
    _u32_t  eflags;
    _u32_t  eax;
    _u32_t  ecx;
    _u32_t  edx;
    _u32_t  ebx;
    _u32_t  esp;
    _u32_t  ebp;
    _u32_t  esi;
    _u32_t  edi;
    _u16_t  es;
    _u16_t  reserved_5;
    _u16_t  cs;
    _u16_t  reserved_6;
    _u16_t  ss;
    _u16_t  reserved_7;
    _u16_t  ds;
    _u16_t  reserved_8;
    _u16_t  fs;
    _u16_t  reserved_9;
    _u16_t  gs;
    _u16_t  reserved_10;
    _u16_t  ldtr;
    _u16_t  reserved_11;
    _u16_t  t;
    _u16_t  iopb_offset;
} __attribute__((packed)) _task_state_segment_t;

_task_state_segment_t globl_tss;

#define load_tss(ptr)    asm volatile ("ltr %%ax": :"a" (ptr))

#endif