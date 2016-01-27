// arch/x86/selector.h

#ifndef _ARCH_X86_SELECTOR_H_
#define _ARCH_X86_SELECTOR_H_

#define SEL_RPL0    0x0
#define SEL_RPL1    0x1
#define SEL_RPL2    0x2
#define SEL_RPL3    0x3
#define SEL_TI      0x4

#define selector(n, attribute)  ((n << 3) | attribute)

#endif