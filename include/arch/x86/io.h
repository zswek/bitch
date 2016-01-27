// arch/x86/io.h

#ifndef _ARCH_X86_IO_H_
#define _ARCH_X86_IO_H_

#include <bits/types.h>

#define outb(data, port) \
asm volatile ( "outb %%al, %%dx": : "a" (data), "d" (port))

#define outw(data, port) \
asm volatile ( "outw %%ax, %%dx": : "a" (data), "d" (port))

#define outb_p(data, port) \
asm volatile ( "outb %%al, %%dx\n" \
    "\tnop\n" \
    "\tnop\n": : "a" (data), "d" (port))

#define outw_p(data, port) \
asm volatile ( "outw %%ax, %%dx\n" \
    "\tnop\n" \
    "\tnop\n": : "a" (data), "d" (port))

#define inb(port) ({ \
_u8_t   ret; \
asm volatile ( "inb %%dx, %%al": "=a" (ret): "d" (port)); \
ret; })

#define inw(port) ({ \
_u16_t  ret; \
asm volatile ( "inw %%dx, %%ax": "=a" (ret): "d" (port)); \
ret; })

#define inb_p(port) ({ \
_u8_t   ret; \
asm volatile ( "inb %%dx, %%al\n" \
    "\tnop\n" \
    "\tnop\n": "=a" (ret): "d" (port)); \
ret; })

#define inw_p(port) ({ \
_u16_t  ret; \
asm volatile ( "inw %%dx, %%ax\n" \
    "\tnop\n" \
    "\tnop\n": "=a" (ret): "d" (port)); \
ret; })

#endif