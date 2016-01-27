// kernel/dev/console.h

#ifndef _KERNEL_DEV_CONSOLE_H_
#define _KERNEL_DEV_CONSOLE_H_

void console_putchar(char c);
void console_write(char *c);
void console_puts(char *c);
void console_clear(void);

void console_init(void);

#endif