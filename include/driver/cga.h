// driver/cga.h

#ifndef _DRIVER_CGA_H_
#define _DRIVER_CGA_H_

#include <bits/types.h>

void init_cga(void);

_size_t get_screen_columns(void);
_size_t get_screen_lines(void);
void screen_wirte(char c, _size_t x, _size_t y);
void screen_scroll(_size_t line);
void screen_clear(void);
void set_video_color(_u8_t color);
void set_cursor(_size_t x, _size_t y);

#endif