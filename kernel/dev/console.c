// kernel/dev/console.c

#include <driver/cga.h>

#define TAB_SIZE    8

static _size_t columns;
static _size_t lines;

static _size_t xpos;
static _size_t ypos;

void console_putchar(char c) {
    
    if (c == '\b') {
        if (xpos != 0) {
            xpos--;
        }
        screen_wirte(0, xpos, ypos);
    }
    else if (c == '\t') {
        xpos = (xpos + TAB_SIZE) & ~(TAB_SIZE - 1);
        if (xpos >= columns) {
            xpos = columns -1;
        }
    }
    else if (c == '\r') {
        xpos = 0;
    }
    else if (c == '\n') {
        xpos = 0;
        ypos++;
    }
    else if (c >= 0x20) {
        screen_wirte(c, xpos, ypos);
        xpos++;
    }
    
    if (xpos >= columns) {
        xpos = 0;
        ypos++;
    }
    
    if (ypos >= lines) {
        screen_scroll(1);
        ypos = lines - 1;
    }
    
    set_cursor(xpos, ypos);
    
}

void console_write(char *c) {
    
    _size_t i = 0;
    while (c[i]) {
        console_putchar(c[i++]);
    }
    
}

void console_puts(char *c) {
    
    console_write(c);
    
    console_putchar('\n');
    
}

void console_clear(void) {
    
    screen_clear();
    
    xpos = 0;
    ypos = 0;
    
    set_cursor(xpos, ypos);
    
    screen_wirte(0, xpos, ypos);
    
}

void console_init(void) {
    
    init_cga();
    
    columns = get_screen_columns();
    lines = get_screen_lines();
    
    xpos = 0;
    ypos = 0;
    
}