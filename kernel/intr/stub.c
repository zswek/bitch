// kernel/intr/stub.c

#include <kernel/def.h>
#include <kernel/intr/idt.h>
#include <kernel/intr/stub.h>

#define set_intr(N)   void int_ ## N();idt_set((N), int_ ## N)

static _intr_handler_t intr_handler_list[IDT_LENGTH];

void int_handler(_intr_regs_t *regs) {
    if (intr_handler_list[regs->int_code] != NULL) {
        intr_handler_list[regs->int_code](regs);
    }
}

void reg_intr_handler(_u8_t n, _intr_handler_t handler) {
    if (n < IDT_LENGTH) {
        intr_handler_list[n] = handler;
    }
}

void stub_init(void) {
    
    int i;
    for (i = 0; i < IDT_LENGTH; i++) {
        intr_handler_list[i] = NULL;
    }
    
    set_intr(0x00);
    set_intr(0x01);
    set_intr(0x02);
    set_intr(0x03);
    set_intr(0x04);
    set_intr(0x05);
    set_intr(0x06);
    set_intr(0x07);
    set_intr(0x08);
    set_intr(0x09);
    set_intr(0x0a);
    set_intr(0x0b);
    set_intr(0x0c);
    set_intr(0x0d);
    set_intr(0x0e);
    set_intr(0x0f);
    set_intr(0x10);
    set_intr(0x11);
    set_intr(0x12);
    set_intr(0x13);
    
    set_intr(0x14);
    set_intr(0x15);
    set_intr(0x16);
    set_intr(0x17);
    set_intr(0x18);
    set_intr(0x19);
    set_intr(0x1a);
    set_intr(0x1b);
    set_intr(0x1c);
    set_intr(0x1d);
    set_intr(0x1e);
    set_intr(0x1f);
    set_intr(0x20);
    
    
    set_intr(0x21);
    set_intr(0x22);
    set_intr(0x23);
    set_intr(0x24);
    set_intr(0x25);
    set_intr(0x26);
    set_intr(0x27);
    set_intr(0x28);
    set_intr(0x29);
    set_intr(0x2a);
    set_intr(0x2b);
    set_intr(0x2c);
    set_intr(0x2d);
    set_intr(0x2e);
    set_intr(0x2f);
    
    set_intr(0x30);
    set_intr(0x31);
    set_intr(0x32);
    set_intr(0x33);
    set_intr(0x34);
    set_intr(0x35);
    set_intr(0x36);
    set_intr(0x37);
    set_intr(0x38);
    set_intr(0x39);
    set_intr(0x3a);
    set_intr(0x3b);
    set_intr(0x3c);
    set_intr(0x3d);
    set_intr(0x3e);
    set_intr(0x3f);
    
    set_intr(0x40);
    set_intr(0x41);
    set_intr(0x42);
    set_intr(0x43);
    set_intr(0x44);
    set_intr(0x45);
    set_intr(0x46);
    set_intr(0x47);
    set_intr(0x48);
    set_intr(0x49);
    set_intr(0x4a);
    set_intr(0x4b);
    set_intr(0x4c);
    set_intr(0x4d);
    set_intr(0x4e);
    set_intr(0x4f);
    
    set_intr(0x50);
    set_intr(0x51);
    set_intr(0x52);
    set_intr(0x53);
    set_intr(0x54);
    set_intr(0x55);
    set_intr(0x56);
    set_intr(0x57);
    set_intr(0x58);
    set_intr(0x59);
    set_intr(0x5a);
    set_intr(0x5b);
    set_intr(0x5c);
    set_intr(0x5d);
    set_intr(0x5e);
    set_intr(0x5f);
    
    set_intr(0x60);
    set_intr(0x61);
    set_intr(0x62);
    set_intr(0x63);
    set_intr(0x64);
    set_intr(0x65);
    set_intr(0x66);
    set_intr(0x67);
    set_intr(0x68);
    set_intr(0x69);
    set_intr(0x6a);
    set_intr(0x6b);
    set_intr(0x6c);
    set_intr(0x6d);
    set_intr(0x6e);
    set_intr(0x6f);
    
    set_intr(0x70);
    set_intr(0x71);
    set_intr(0x72);
    set_intr(0x73);
    set_intr(0x74);
    set_intr(0x75);
    set_intr(0x76);
    set_intr(0x77);
    set_intr(0x78);
    set_intr(0x79);
    set_intr(0x7a);
    set_intr(0x7b);
    set_intr(0x7c);
    set_intr(0x7d);
    set_intr(0x7e);
    set_intr(0x7f);
    
    set_intr(0x80);
    set_intr(0x81);
    set_intr(0x82);
    set_intr(0x83);
    set_intr(0x84);
    set_intr(0x85);
    set_intr(0x86);
    set_intr(0x87);
    set_intr(0x88);
    set_intr(0x89);
    set_intr(0x8a);
    set_intr(0x8b);
    set_intr(0x8c);
    set_intr(0x8d);
    set_intr(0x8e);
    set_intr(0x8f);
    
    set_intr(0x90);
    set_intr(0x91);
    set_intr(0x92);
    set_intr(0x93);
    set_intr(0x94);
    set_intr(0x95);
    set_intr(0x96);
    set_intr(0x97);
    set_intr(0x98);
    set_intr(0x99);
    set_intr(0x9a);
    set_intr(0x9b);
    set_intr(0x9c);
    set_intr(0x9d);
    set_intr(0x9e);
    set_intr(0x9f);
    
    set_intr(0xa0);
    set_intr(0xa1);
    set_intr(0xa2);
    set_intr(0xa3);
    set_intr(0xa4);
    set_intr(0xa5);
    set_intr(0xa6);
    set_intr(0xa7);
    set_intr(0xa8);
    set_intr(0xa9);
    set_intr(0xaa);
    set_intr(0xab);
    set_intr(0xac);
    set_intr(0xad);
    set_intr(0xae);
    set_intr(0xaf);
    
    set_intr(0xb0);
    set_intr(0xb1);
    set_intr(0xb2);
    set_intr(0xb3);
    set_intr(0xb4);
    set_intr(0xb5);
    set_intr(0xb6);
    set_intr(0xb7);
    set_intr(0xb8);
    set_intr(0xb9);
    set_intr(0xba);
    set_intr(0xbb);
    set_intr(0xbc);
    set_intr(0xbd);
    set_intr(0xbe);
    set_intr(0xbf);
    
    set_intr(0xc0);
    set_intr(0xc1);
    set_intr(0xc2);
    set_intr(0xc3);
    set_intr(0xc4);
    set_intr(0xc5);
    set_intr(0xc6);
    set_intr(0xc7);
    set_intr(0xc8);
    set_intr(0xc9);
    set_intr(0xca);
    set_intr(0xcb);
    set_intr(0xcc);
    set_intr(0xcd);
    set_intr(0xce);
    set_intr(0xcf);
    
    set_intr(0xd0);
    set_intr(0xd1);
    set_intr(0xd2);
    set_intr(0xd3);
    set_intr(0xd4);
    set_intr(0xd5);
    set_intr(0xd6);
    set_intr(0xd7);
    set_intr(0xd8);
    set_intr(0xd9);
    set_intr(0xda);
    set_intr(0xdb);
    set_intr(0xdc);
    set_intr(0xdd);
    set_intr(0xde);
    set_intr(0xdf);
    
    set_intr(0xe0);
    set_intr(0xe1);
    set_intr(0xe2);
    set_intr(0xe3);
    set_intr(0xe4);
    set_intr(0xe5);
    set_intr(0xe6);
    set_intr(0xe7);
    set_intr(0xe8);
    set_intr(0xe9);
    set_intr(0xea);
    set_intr(0xeb);
    set_intr(0xec);
    set_intr(0xed);
    set_intr(0xee);
    set_intr(0xef);
    
    set_intr(0xf0);
    set_intr(0xf1);
    set_intr(0xf2);
    set_intr(0xf3);
    set_intr(0xf4);
    set_intr(0xf5);
    set_intr(0xf6);
    set_intr(0xf7);
    set_intr(0xf8);
    set_intr(0xf9);
    set_intr(0xfa);
    set_intr(0xfb);
    set_intr(0xfc);
    set_intr(0xfd);
    set_intr(0xfe);
    set_intr(0xff);
    
}