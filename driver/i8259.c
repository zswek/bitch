// driver/i8259.c

#include <arch/x86/io.h>
#include <kernel/intr/stub.h>
#include <driver/i8259.h>

#define I8259_M_CTL_IO  0x20
#define I8259_M_SET_IO  0x21
#define I8259_S_CTL_IO  0xa0
#define I8259_S_SET_IO  0xa1

static _u8_t master_int_mask;
static _u8_t slave_int_mask;

static _intr_handler_t irq_handler_list[16];

static void irq_handler(_intr_regs_t *regs) {
    outb(0x20, I8259_M_CTL_IO);
    if (regs -> int_code < S_INT_VECTOR) {
        if (irq_handler_list[regs -> int_code - M_INT_VECTOR] != NULL) {
            irq_handler_list[regs -> int_code - M_INT_VECTOR](regs);
        }
    }
    else {
        outb(0x20, I8259_S_CTL_IO);
        if (irq_handler_list[regs -> int_code - S_INT_VECTOR + 8] != NULL) {
            irq_handler_list[regs -> int_code - S_INT_VECTOR + 8](regs);
        }
    }
}

void reg_irq_handler(_size_t n, _intr_handler_t handler) {
    if (n < 16) {
        irq_handler_list[n] = handler;
        if (n < 8) {
            master_int_mask &= ~(0x1 << n);
            outb(master_int_mask, I8259_M_SET_IO);
        }
        else {
            slave_int_mask &= ~(0x1 << (n - 8));
            outb(slave_int_mask, I8259_S_SET_IO);
        }
    }
}

void init_8259A(void) {
    
    master_int_mask = 0xff;
    slave_int_mask = 0xff;

    int i;
    for (i = 0; i < 8; i++) {
        irq_handler_list[i] = NULL;
        irq_handler_list[i + 8] = NULL;
        reg_intr_handler(M_INT_VECTOR + i, irq_handler);
        reg_intr_handler(S_INT_VECTOR + i, irq_handler);
    }
    
    outb_p(0x11, I8259_M_CTL_IO);
    outb_p(0x11, I8259_S_CTL_IO);
    
    outb_p(0x20, I8259_M_SET_IO);
    outb_p(0x28, I8259_S_SET_IO);
    
    outb_p(0x04, I8259_M_SET_IO);
    outb_p(0x02, I8259_S_SET_IO);
    
    outb_p(0x01, I8259_M_SET_IO);
    outb_p(0x01, I8259_S_SET_IO);
    
    outb_p(master_int_mask, I8259_M_SET_IO);
    outb_p(slave_int_mask, I8259_S_SET_IO);
    
    reg_irq_handler(2, NULL);
}