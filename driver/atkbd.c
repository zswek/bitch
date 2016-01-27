// driver/atkbd.c

#include <arch/x86/io.h>
#include <driver/i8259.h>
#include <driver/atkbd.h>
#include <kernel/process/lock.h>

#define I8042_DATA_IO   0x60
#define I8042_STAT_IO   0x64
#define I8042_CTL_IO    0x64

#define SET_LED         0xed

static _key_handler_t atkbd_handler;

static _lock_t ilock;

static void irq1_handler(_intr_regs_t *regs) {
    
    _u8_t scan_code = inb(I8042_DATA_IO);
    
    intr_unlock(&ilock);
    
    if (atkbd_handler != NULL) {
        atkbd_handler(scan_code);
    }
    
}

void set_kbd_led(_u8_t c) {
    while (inb(I8042_STAT_IO) & 0x2) {}
    
led_1:
    
    init_intr_lock(&ilock);
    
    outb(SET_LED, I8042_DATA_IO);
    
    intr_lock(&ilock);
    
    if (inb(I8042_DATA_IO) == 0xfe) goto led_1;
    
led_2:
    
    init_intr_lock(&ilock);
    
    outb(c, I8042_DATA_IO);
    
    intr_lock(&ilock);
    
    if (inb(I8042_DATA_IO) == 0xfe) goto led_2;
    
}

void reg_atkbd_handler(_key_handler_t handler) {
    
    atkbd_handler = handler;
    
}

void init_atkbd(void) {
    
    atkbd_handler = NULL;
    
    reg_irq_handler(1, irq1_handler);
    
}