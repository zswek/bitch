// driver/cga.c

#include <arch/x86/io.h>
#include <driver/cga.h>

#define CRTC_ADDR_IO    0x3d4
#define CRTC_DATA_IO    0x3d5

#define CGA_COLUMNS     80
#define CGA_LINES       25
#define CGA_ATTRIBUTE   0x07
#define CGA_MEM_ATTR    0xb8000

#define CURSOR_H_REG    0x0e
#define CURSOR_L_REG    0x0f

static _u8_t attribute = 0x07;

_size_t get_screen_columns(void) {
    return CGA_COLUMNS;
}

_size_t get_screen_lines(void) {
    return CGA_LINES;
}

void screen_wirte(char c, _size_t x, _size_t y) {
    
    *((_u8_t*) (CGA_MEM_ATTR + (x + y * CGA_COLUMNS) * 2)) = c & 0xff;
    *((_u8_t*) (CGA_MEM_ATTR + (x + y * CGA_COLUMNS) * 2 + 1)) = attribute & 0xff;
    
}

void screen_scroll(_size_t line) {
    
    int i;
    for (i = 0; i < CGA_COLUMNS * (CGA_LINES - line); i++) {
        *((_u16_t*) (CGA_MEM_ATTR) + i) = *((_u16_t*) (CGA_MEM_ATTR) + i + line * 80);
    }
    
    for (; i < CGA_COLUMNS * CGA_LINES; i++) {
        *((_u16_t*) (CGA_MEM_ATTR) + i) = attribute << 8 & 0xff00;
    }
    
}

void screen_clear(void) {
    
    int i;
    for (i = 0; i < CGA_COLUMNS * CGA_LINES; i++) {
        *((_u16_t*) (CGA_MEM_ATTR) + i) = attribute << 8 & 0xff00;
    }
    
}

void set_video_color(_u8_t color) {
    
    attribute = color;
    
}

void set_cursor(_size_t x, _size_t y) {
    
    _u16_t location = y * CGA_COLUMNS + x;
    
    outb_p(CURSOR_H_REG, CRTC_ADDR_IO);
    outb_p(location >> 8, CRTC_DATA_IO);
    outb_p(CURSOR_L_REG, CRTC_ADDR_IO);
    outb_p(location, CRTC_DATA_IO);
    
}

void init_cga(void) {
    
    attribute = CGA_ATTRIBUTE;
    
    screen_clear();
    
    set_cursor(0, 0);
    
}