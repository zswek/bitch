// kernel/dev/keyboard.h

#ifndef _KERNEL_DEV_KEYBOARD_H_
#define _KERNEL_DEV_KAYBOARD_H_

#include <bits/types.h>

#define KEY_FLAG_EXT        0x80
#define KEY_FLAG_BREAK      0x100

#define KEY_ESC_INPUT           '\x1b'
#define KEY_TAB_INPUT           '\t'
#define KEY_ENTER_INPUT         '\n'
#define KEY_BACKSPACE_INPUT     '\b'

#define KEY_ESC                 (0x01 + KEY_FLAG_EXT)
#define KEY_TAB                 (0x02 + KEY_FLAG_EXT)
#define KEY_ENTER               (0x03 + KEY_FLAG_EXT)
#define KEY_BACKSPACE           (0x04 + KEY_FLAG_EXT)

#define KEY_GUI_L               (0x05 + KEY_FLAG_EXT)
#define KEY_GUI_R               (0x06 + KEY_FLAG_EXT)
#define KEY_APPS                (0x07 + KEY_FLAG_EXT)

#define KEY_SHIFT_L             (0x08 + KEY_FLAG_EXT)
#define KEY_SHIFT_R             (0x09 + KEY_FLAG_EXT)
#define KEY_CTRL_L              (0x0a + KEY_FLAG_EXT)
#define KEY_CTRL_R              (0x0b + KEY_FLAG_EXT)
#define KEY_ALT_L               (0x0c + KEY_FLAG_EXT)
#define KEY_ALT_R               (0x0d + KEY_FLAG_EXT)

#define KEY_CAPS_LOCK           (0x0e + KEY_FLAG_EXT)
#define KEY_NUM_LOCK            (0x0f + KEY_FLAG_EXT)
#define KEY_SCROLL_LOCK         (0x10 + KEY_FLAG_EXT)

#define KEY_F1                  (0x11 + KEY_FLAG_EXT)
#define KEY_F2                  (0x12 + KEY_FLAG_EXT)
#define KEY_F3                  (0x13 + KEY_FLAG_EXT)
#define KEY_F4                  (0x14 + KEY_FLAG_EXT)
#define KEY_F5                  (0x15 + KEY_FLAG_EXT)
#define KEY_F6                  (0x16 + KEY_FLAG_EXT)
#define KEY_F7                  (0x17 + KEY_FLAG_EXT)
#define KEY_F8                  (0x18 + KEY_FLAG_EXT)
#define KEY_F9                  (0x19 + KEY_FLAG_EXT)
#define KEY_F10                 (0x1a + KEY_FLAG_EXT)
#define KEY_F11                 (0x1b + KEY_FLAG_EXT)
#define KEY_F12                 (0x1c + KEY_FLAG_EXT)

#define KEY_PRINTSCREEN         (0x1d + KEY_FLAG_EXT)
#define KEY_PAUSEBREAK          (0x1e + KEY_FLAG_EXT)
#define KEY_INSERT              (0x1f + KEY_FLAG_EXT)
#define KEY_DELETE              (0x20 + KEY_FLAG_EXT)
#define KEY_HOME                (0x21 + KEY_FLAG_EXT)
#define KEY_END                 (0x22 + KEY_FLAG_EXT)
#define KEY_PAGEUP              (0x23 + KEY_FLAG_EXT)
#define KEY_PAGEDOWN            (0x24 + KEY_FLAG_EXT)
#define KEY_UP                  (0x25 + KEY_FLAG_EXT)
#define KEY_DOWN                (0x26 + KEY_FLAG_EXT)
#define KEY_LEFT                (0x27 + KEY_FLAG_EXT)
#define KEY_RIGHT               (0x28 + KEY_FLAG_EXT)

#define KEY_POWER               (0x29 + KEY_FLAG_EXT)
#define KEY_SLEEP               (0x2a + KEY_FLAG_EXT)
#define KEY_WAKE                (0x2b + KEY_FLAG_EXT)

#define KEY_PAD_SLASH           (0x2c + KEY_FLAG_EXT)
#define KEY_PAD_STAR            (0x2d + KEY_FLAG_EXT)
#define KEY_PAD_MINUS           (0x2e + KEY_FLAG_EXT)
#define KEY_PAD_PLUS            (0x2f + KEY_FLAG_EXT)
#define KEY_PAD_ENTER           (0x30 + KEY_FLAG_EXT)
#define KEY_PAD_DOT             (0x31 + KEY_FLAG_EXT)
#define KEY_PAD_0               (0x32 + KEY_FLAG_EXT)
#define KEY_PAD_1               (0x33 + KEY_FLAG_EXT)
#define KEY_PAD_2               (0x34 + KEY_FLAG_EXT)
#define KEY_PAD_3               (0x35 + KEY_FLAG_EXT)
#define KEY_PAD_4               (0x36 + KEY_FLAG_EXT)
#define KEY_PAD_5               (0x37 + KEY_FLAG_EXT)
#define KEY_PAD_6               (0x38 + KEY_FLAG_EXT)
#define KEY_PAD_7               (0x39 + KEY_FLAG_EXT)
#define KEY_PAD_8               (0x3a + KEY_FLAG_EXT)
#define KEY_PAD_9               (0x3b + KEY_FLAG_EXT)
#define KEY_PAD_UP              KEY_PAD_8
#define KEY_PAD_DOWN            KEY_PAD_2
#define KEY_PAD_LEFT            KEY_PAD_4
#define KEY_PAD_RIGHT           KEY_PAD_6
#define KEY_PAD_HOME            KEY_PAD_7
#define KEY_PAD_END             KEY_PAD_1
#define KEY_PAD_PAGEUP          KEY_PAD_9
#define KEY_PAD_PAGEDOWN        KEY_PAD_3
#define KEY_PAD_INS             KEY_PAD_0
#define KEY_PAD_MID             KEY_PAD_5
#define KEY_PAD_DEL             KEY_PAD_DOT

void set_input_mode(_u8_t m);

void reg_key_callback(void (*key_callback)(_u16_t));

void keyboard_init(void);

#endif