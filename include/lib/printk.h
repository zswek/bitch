// lib/printk.h

#ifndef _LIB_PRINTK_H_
#define _LIB_PRINTK_H_

#include <lib/stdarg.h>

int printk(const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);

#endif