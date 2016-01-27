// kernel/dev/keyboard.c

#include <driver/atkbd.h>
#include <kernel/dev/keyboard.h>
#include <kernel/dev/console.h>

#define CODE_BREAK      0x80

#define CODE_EMUL0      0xe0
#define CODE_EMUL1      0xe1

#define FLAG_EMUL0      0x01
#define FLAG_EMUL1      0x02

#define FLAG_SHIFT_L    0x04
#define FLAG_SHIFT_R    0x08

#define FLAG_CTRL_L      0x10
#define FLAG_CTRL_R      0x20
#define FLAG_ALT_L       0x40
#define FLAG_ALT_R       0x80

static _u8_t keyboard_map[0x80][3] = {
{0,0,0},
{KEY_ESC,KEY_ESC,0},
{'1','!',0},
{'2','@',0},
{'3','#',0},
{'4','$',0},
{'5','%',0},
{'6','^',0},
{'7','&',0},
{'8','*',0},
{'9','(',0},
{'0',')',0},
{'-','_',0},
{'=','+',0},
{KEY_BACKSPACE,KEY_BACKSPACE,0},
{KEY_TAB,KEY_TAB,0},
{'q','Q',0},
{'w','W',0},
{'e','E',0},
{'r','R',0},
{'t','T',0},
{'y','Y',0},
{'u','U',0},
{'i','I',0},
{'o','O',0},
{'p','P',0},
{'[','{',0},
{']','}',0},
{KEY_ENTER,KEY_ENTER,KEY_PAD_ENTER},
{KEY_CTRL_L,KEY_CTRL_L,KEY_CTRL_R},
{'a','A',0},
{'s','S',0},
{'d','D',0},
{'f','F',0},
{'g','G',0},
{'h','H',0},
{'j','J',0},
{'k','K',0},
{'l','L',0},
{';',':',0},
{'\'','"',0},
{'`','~',0},
{KEY_SHIFT_L,KEY_SHIFT_L,0},
{'\\','|',0},
{'z','Z',0},
{'x','X',0},
{'c','C',0},
{'v','V',0},
{'b','B',0},
{'n','N',0},
{'m','M',0},
{',','<',0},
{'.','>',0},
{'/','?',KEY_PAD_SLASH},
{KEY_SHIFT_R,KEY_SHIFT_R,0},
{'*','*',    0},
{KEY_ALT_L,KEY_ALT_L,  KEY_ALT_R},
{' ',' ',0},
{KEY_CAPS_LOCK,KEY_CAPS_LOCK,0},
{KEY_F1,KEY_F1,0},
{KEY_F2,KEY_F2,0},
{KEY_F3,KEY_F3,0},
{KEY_F4,KEY_F4,0},
{KEY_F5,KEY_F5,0},
{KEY_F6,KEY_F6,0},
{KEY_F7,KEY_F7,0},
{KEY_F8,KEY_F8,0},
{KEY_F9,KEY_F9,0},
{KEY_F10,KEY_F10,0},
{KEY_NUM_LOCK,KEY_NUM_LOCK,0},
{KEY_SCROLL_LOCK,KEY_SCROLL_LOCK,0},
{KEY_PAD_HOME,'7',KEY_HOME},
{KEY_PAD_UP,'8',KEY_UP},
{KEY_PAD_PAGEUP,'9',KEY_PAGEUP},
{KEY_PAD_MINUS,'-',0},
{KEY_PAD_LEFT,'4',KEY_LEFT},
{KEY_PAD_MID,'5',0},
{KEY_PAD_RIGHT,'6',KEY_RIGHT},
{KEY_PAD_PLUS,'+',0},
{KEY_PAD_END,'1',KEY_END},
{KEY_PAD_DOWN,'2',KEY_DOWN},
{KEY_PAD_PAGEDOWN,'3',KEY_PAGEDOWN},
{KEY_PAD_INS,'0',KEY_INSERT},
{KEY_PAD_DOT,'.',KEY_DELETE},
{0,0,0},
{0,0,0},
{0,0,0},
{KEY_F11,KEY_F11,0},
{KEY_F12,KEY_F12,0},
{0,0,0},
{0,0,0},
{0,0,KEY_GUI_L},
{0,0,KEY_GUI_R},
{0,0,KEY_APPS},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0},
{0,0,0}
};

static void (*key_callback)(_u16_t);
static _u16_t key_flags;
static _u8_t key_led;
static _u8_t input_mode;

static void keyboard_handler(_u8_t code) {
    
    if (code == CODE_EMUL0) {
        key_flags |= FLAG_EMUL0;
    }
    else if (code == CODE_EMUL1) {
        key_flags |= FLAG_EMUL1;
    }
    else {
        _u16_t key = 0;
        _u8_t flags = 0;
        if ((key_flags & FLAG_EMUL0) == FLAG_EMUL0) {
            flags = 2;
            key_flags &= ~FLAG_EMUL0;
        }
        if (code > CODE_BREAK) {
            key = keyboard_map[code - CODE_BREAK][flags];
            if (key == KEY_SHIFT_L) {
                key_flags &= ~FLAG_SHIFT_L;
            }
            else if (key == KEY_SHIFT_R) {
                key_flags &= ~FLAG_SHIFT_R;
            }
            else if (key == KEY_CTRL_L) {
                key_flags &= ~FLAG_CTRL_L;
            }
            else if (key == KEY_CTRL_R) {
                key_flags &= ~FLAG_CTRL_R;
            }
            else if (key == KEY_ALT_L) {
                key_flags &= ~FLAG_ALT_L;
            }
            else if (key == KEY_ALT_R) {
                key_flags &= ~FLAG_ALT_R;
            }
            if (input_mode == 0) {
                key += KEY_FLAG_BREAK;
            }
            else {
                key = 0;
            }
        }
        else {
            key = keyboard_map[code][flags];
            if (key == KEY_SHIFT_L) {
                key_flags |= FLAG_SHIFT_L;
            }
            else if (key == KEY_SHIFT_R) {
                key_flags |= FLAG_SHIFT_R;
            }
            else if (key == KEY_CTRL_L) {
                key_flags |= FLAG_CTRL_L;
            }
            else if (key == KEY_CTRL_R) {
                key_flags |= FLAG_CTRL_R;
            }
            else if (key == KEY_ALT_L) {
                key_flags |= FLAG_ALT_L;
            }
            else if (key == KEY_ALT_R) {
                key_flags |= FLAG_ALT_R;
            }
            if (input_mode != 0) {
                if ((key_flags & FLAG_SHIFT_L) == FLAG_SHIFT_L || (key_flags & FLAG_SHIFT_R) == FLAG_SHIFT_R) {
                    key = keyboard_map[code][1];
                }
                if ((key_flags & FLAG_CTRL_L) == FLAG_CTRL_L || (key_flags & FLAG_CTRL_R) == FLAG_CTRL_R || 
                (key_flags & FLAG_ALT_L) == FLAG_ALT_L || (key_flags & FLAG_ALT_R) == FLAG_ALT_R) {
                    key = 0;
                }
                if (key == KEY_TAB) {
                    key = KEY_TAB_INPUT;
                }
                else if (key == KEY_ENTER) {
                    key = KEY_ENTER_INPUT;
                }
                else if (key == KEY_BACKSPACE) {
                    key = KEY_BACKSPACE_INPUT;
                }
                if (key > KEY_FLAG_EXT) {
                    key = 0;
                }
                if (key != 0 && input_mode == 2) {
                    console_putchar(key);
                }
            }
        }
        if (key != 0 && key_callback != NULL) {
            key_callback(key);
        }
    }
    
}

void reg_key_callback(void (*callback)(_u16_t)) {
    
    key_callback = callback;
    
}

void set_input_mode(_u8_t m) {
    
    input_mode = m;
    
}

void keyboard_init(void) {
    
    key_callback = NULL;
    
    key_flags = 0;
    
    key_led = 0;
    
    input_mode = 0;
    
    init_atkbd();
    
    reg_atkbd_handler(keyboard_handler);
    
    set_kbd_led(0);
    
}
