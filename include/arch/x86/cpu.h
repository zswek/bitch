// arch/x86/cpu.h

#ifndef _ARCH_X86_CPU_H_
#define _ARCH_X86_CPU_H_

#define enable_intr()   asm volatile ("sti")
#define disable_intr()  asm volatile ("cli")

#define intr(n)  asm volatile ("int %0": :"i"(n))

#define halt()  asm volatile ("hlt")

#endif