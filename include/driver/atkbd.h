// driver/atkbd.h

#ifndef _DRIVER_ATKBD_H_
#define _DRIVER_ATKBD_H_

#include <asm/types.h>
#include <driver/i8259.h>

#define KEYBOARD_INT    IRQ0_INT

typedef void (*_key_handler_t)(_u8_t);

void set_kbd_led(_u8_t c);

void reg_atkbd_handler(_key_handler_t handler);

void init_atkbd(void);

#endif